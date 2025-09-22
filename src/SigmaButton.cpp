#include "SigmaButton.h"  

ESP_EVENT_DEFINE_BASE(SIGMA_BUTTON_EVENT);

SigmaButton::SigmaButton(byte btnId, int pin, bool clickLevel, bool pullup)
{
    this->btnId = btnId;
    this->pin = pin;
    this->clickLevel = clickLevel;
    this->timeDebounce = timeDebounce;
    PinMode(pin, pullup ? INPUT_PULLUP : INPUT);
    btnStatus = DigitalRead(pin);
    AttachInterruptArg(pin, processISR, this, CHANGE);
    debounceTimer = xTimerCreateStatic("SB_debounce", pdMS_TO_TICKS(timeDebounce), pdFALSE, this, debounceFunc, &debounceTimerBuffer);
    cycleTimer = xTimerCreateStatic("SB_cycle", pdMS_TO_TICKS(100), pdFALSE, this, cycleFunc, &cycleTimerBuffer);
    doubleTimer = xTimerCreateStatic("SB_double", pdMS_TO_TICKS(timeDouble), pdFALSE, this, doubleFunc, &doubleTimerBuffer);
}

SigmaButton::~SigmaButton()
{
    detachInterrupt(pin);
    xTimerDelete(debounceTimer, 0);
    xTimerDelete(cycleTimer, 0);
    xTimerDelete(doubleTimer, 0);
}

IRAM_ATTR void SigmaButton::processISR(void *arg)
{
    SigmaButton *button = (SigmaButton *)arg;
    button->process();
}

void SigmaButton::process()
{
    DetachInterrupt(pin);
    bool btn = DigitalRead(pin);
    if (btn != btnStatus)
    {
        btnStatus = btn;
        btnClick = (clickLevel == btnStatus);
        xTimerStart(debounceTimer, 0);
    }
    else
    {
        AttachInterruptArg(pin, processISR, this, CHANGE);
    }
}

void SigmaButton::doubleFunc(TimerHandle_t xTimer)
{
    SigmaButton *button = (SigmaButton *)pvTimerGetTimerID(xTimer);
    button->btnState = SIGMA_BUTTON_EVENT_RELEASED;
    esp_event_post(SIGMA_BUTTON_EVENT, SIGMA_BUTTON_EVENT_RELEASED, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
}

void SigmaButton::debounceFunc(TimerHandle_t xTimer)
{
    SigmaButton *button = (SigmaButton *)pvTimerGetTimerID(xTimer);
    ulong ts = millis();
    Serial.printf("[%lu]Debounce: %d:%d\n", ts, button->btnClick, button->btnStatus);
    bool btn = button->DigitalRead(button->pin);
    //   Serial.printf("btn: %d\n", btn);
    if (btn == button->btnStatus)
    { // action confirmed
        if (button->btnClick)
        {
            ulong ts = millis();
            if (button->lastClick + button->timeDouble > ts)
            {
                button->btnState = SIGMA_BUTTON_EVENT_DOUBLE_CLICK;
                esp_event_post(SIGMA_BUTTON_EVENT, button->btnState, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
                xTimerStop(button->doubleTimer, 0);
            }
            else
            {
                button->btnState = SIGMA_BUTTON_EVENT_CLICK;
                esp_event_post(SIGMA_BUTTON_EVENT, button->btnState, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
                xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeSimple), 0);
                xTimerStart(button->cycleTimer, 0);
            }
            button->lastClick = ts;
        }
        else
        {
            xTimerStop(button->cycleTimer, 0);
            if (button->btnState == SIGMA_BUTTON_EVENT_CLICK)
            {
                xTimerStart(button->doubleTimer, 0);
            }
            else
            {
                button->btnState = button->releaseEvent();
                esp_event_post(SIGMA_BUTTON_EVENT, button->btnState, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
            }
        }
    }
    else
    {
        esp_event_post(SIGMA_BUTTON_EVENT, SIGMA_BUTTON_EVENT_ERROR, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
    }
    button->AttachInterruptArg(button->pin, processISR, button, CHANGE);
}

SigmaButtonEvent SigmaButton::releaseEvent()
{
    if (btnState == SIGMA_BUTTON_EVENT_CLICK)
        return SIGMA_BUTTON_EVENT_RELEASED;
    if (btnState == SIGMA_BUTTON_EVENT_SHORT_PRESSED)
        return SIGMA_BUTTON_EVENT_SIMPLE_RELEASED;
    if (btnState == SIGMA_BUTTON_EVENT_LONG_PRESSED)
        return SIGMA_BUTTON_EVENT_LONG_RELEASED;
    if (btnState == SIGMA_BUTTON_EVENT_LONG_LONG_PRESSED)
        return SIGMA_BUTTON_EVENT_LONG_LONG_RELEASED;
    if (btnState == SIGMA_BUTTON_EVENT_DOUBLE_CLICK)
        return SIGMA_BUTTON_EVENT_DOUBLE_RELEASED;
    //    Serial.printf("STATE=%d\n", btnState);
    return SIGMA_BUTTON_EVENT_ERROR;
}

void SigmaButton::cycleFunc(TimerHandle_t xTimer)
{
    SigmaButton *button = (SigmaButton *)pvTimerGetTimerID(xTimer);
    //    ulong ts = millis();
    //    Serial.printf("[%lu]Cycle: %d\n", ts, button->btnState);
    // bool btn = digitalRead(button->pin);
    switch (button->btnState)
    {
    case SIGMA_BUTTON_EVENT_CLICK:
        button->btnState = SIGMA_BUTTON_EVENT_SHORT_PRESSED;
        esp_event_post(SIGMA_BUTTON_EVENT, button->btnState, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);

        xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeLong), 0);
        xTimerStart(button->cycleTimer, 0);
        break;
    case SIGMA_BUTTON_EVENT_SHORT_PRESSED:
        button->btnState = SIGMA_BUTTON_EVENT_LONG_PRESSED;
        esp_event_post(SIGMA_BUTTON_EVENT, button->btnState, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
        xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeLongLong), 0);
        xTimerStart(button->cycleTimer, 0);
        break;
    case SIGMA_BUTTON_EVENT_LONG_PRESSED:
        button->btnState = SIGMA_BUTTON_EVENT_LONG_LONG_PRESSED;
        esp_event_post(SIGMA_BUTTON_EVENT, button->btnState, &(button->btnId), sizeof(button->btnId), portMAX_DELAY);
        //            xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeDouble), 0);
        //            xTimerStart(button->cycleTimer, 0);
        break;
    }
}

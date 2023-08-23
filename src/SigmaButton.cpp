#include "SigmaButton.hpp"

ESP_EVENT_DEFINE_BASE(SIGMABUTTON_EVENT);

SigmaButton::SigmaButton(int pin, bool clickLevel, int debounceTime, bool pullup)
{
    pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
    this->pin = pin;
    this->clickLevel = clickLevel;
    this->debounceTime = debounceTime;
    btnStatus = digitalRead(pin);
    attachInterruptArg(pin, processISR, this, CHANGE);
    debounceTimer = xTimerCreateStatic("SB_debounce", pdMS_TO_TICKS(debounceTime), pdFALSE, this, debounceFunc, &debounceTimerBuffer);
    cycleTimer = xTimerCreateStatic("SB_cycle", pdMS_TO_TICKS(100), pdFALSE, this, cycleFunc, &cycleTimerBuffer);
    doubleTimer = xTimerCreateStatic("SB_double", pdMS_TO_TICKS(timeDouble), pdFALSE, this, doubleFunc, &doubleTimerBuffer);
}

SigmaButton::~SigmaButton()
{
    detachInterrupt(pin);
    xTimerDelete(debounceTimer, 0);
    xTimerDelete(cycleTimer, 0);
}

IRAM_ATTR void SigmaButton::processISR(void *arg)
{
    SigmaButton *button = (SigmaButton *)arg;
    button->process();
}

void SigmaButton::process()
{
    detachInterrupt(pin);
    bool btn = digitalRead(pin);
    if (btn != btnStatus)
    {
        btnStatus = btn;
        btnClick = (clickLevel == btnStatus);
        xTimerStart(debounceTimer, 0);
    }
    else
    {
        attachInterruptArg(pin, processISR, this, CHANGE);
    }
}

void SigmaButton::doubleFunc(TimerHandle_t xTimer)
{
    SigmaButton *button = (SigmaButton *)pvTimerGetTimerID(xTimer);
    button->btnState = SIGMABUTTON_EVENT_RELEASE;
    esp_event_post(SIGMABUTTON_EVENT, SIGMABUTTON_EVENT_RELEASE, NULL, 0, portMAX_DELAY);
}

void SigmaButton::debounceFunc(TimerHandle_t xTimer)
{
    SigmaButton *button = (SigmaButton *)pvTimerGetTimerID(xTimer);
    ulong ts = millis();
    Serial.printf("[%lu]Debounce: %d:%d\n", ts, button->btnClick, button->btnStatus);
    bool btn = digitalRead(button->pin);
    //   Serial.printf("btn: %d\n", btn);
    if (btn == button->btnStatus)
    { // action confirmed
        if (button->btnClick)
        {
            ulong ts = millis();
            if (button->lastClick + button->timeDouble > ts)
            {
                button->btnState = SIGMABUTTON_EVENT_DOUBLE_CLICK;
                esp_event_post(SIGMABUTTON_EVENT, button->btnState, NULL, 0, portMAX_DELAY);
                xTimerStop(button->doubleTimer, 0);
            }
            else
            {
                button->btnState = SIGMABUTTON_EVENT_CLICK;
                esp_event_post(SIGMABUTTON_EVENT, button->btnState, NULL, 0, portMAX_DELAY);
                xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeSimple), 0);
                xTimerStart(button->cycleTimer, 0);
            }
            button->lastClick = ts;
        }
        else
        {
            xTimerStop(button->cycleTimer, 0);
            if (button->btnState == SIGMABUTTON_EVENT_CLICK)
            {
                xTimerStart(button->doubleTimer, 0);
            }
            else
            {
                button->btnState = button->releaseEvent();
                esp_event_post(SIGMABUTTON_EVENT, button->btnState, NULL, 0, portMAX_DELAY);
            }
        }
    }
    else
    {
        esp_event_post(SIGMABUTTON_EVENT, SIGMABUTTON_EVENT_ERROR, NULL, 0, portMAX_DELAY);
    }
    attachInterruptArg(button->pin, processISR, button, CHANGE);
}

SigmaButtonEvent SigmaButton::releaseEvent()
{
    if (btnState == SIGMABUTTON_EVENT_CLICK)
        return SIGMABUTTON_EVENT_RELEASE;
    if (btnState == SIGMABUTTON_EVENT_SIMPLE_CLICK)
        return SIGMABUTTON_EVENT_SIMPLE_RELEASE;
    if (btnState == SIGMABUTTON_EVENT_LONG_CLICK)
        return SIGMABUTTON_EVENT_LONG_RELEASE;
    if (btnState == SIGMABUTTON_EVENT_LONG_LONG_CLICK)
        return SIGMABUTTON_EVENT_LONG_LONG_RELEASE;
    if (btnState == SIGMABUTTON_EVENT_DOUBLE_CLICK)
        return SIGMABUTTON_EVENT_DOUBLE_RELEASE;
    //    Serial.printf("STATE=%d\n", btnState);
    return SIGMABUTTON_EVENT_ERROR;
}

void SigmaButton::cycleFunc(TimerHandle_t xTimer)
{
    SigmaButton *button = (SigmaButton *)pvTimerGetTimerID(xTimer);
    //    ulong ts = millis();
    //    Serial.printf("[%lu]Cycle: %d\n", ts, button->btnState);
    bool btn = digitalRead(button->pin);
    switch (button->btnState)
    {
    case SIGMABUTTON_EVENT_CLICK:
        button->btnState = SIGMABUTTON_EVENT_SIMPLE_CLICK;
        esp_event_post(SIGMABUTTON_EVENT, button->btnState, NULL, 0, portMAX_DELAY);

        xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeLong), 0);
        xTimerStart(button->cycleTimer, 0);
        break;
    case SIGMABUTTON_EVENT_SIMPLE_CLICK:
        button->btnState = SIGMABUTTON_EVENT_LONG_CLICK;
        esp_event_post(SIGMABUTTON_EVENT, button->btnState, NULL, 0, portMAX_DELAY);
        xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeLongLong), 0);
        xTimerStart(button->cycleTimer, 0);
        break;
    case SIGMABUTTON_EVENT_LONG_CLICK:
        button->btnState = SIGMABUTTON_EVENT_LONG_LONG_CLICK;
        esp_event_post(SIGMABUTTON_EVENT, button->btnState, NULL, 0, portMAX_DELAY);
        //            xTimerChangePeriod(button->cycleTimer, pdMS_TO_TICKS(button->timeDouble), 0);
        //            xTimerStart(button->cycleTimer, 0);
        break;
    }
}

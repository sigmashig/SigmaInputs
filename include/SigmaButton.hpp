#pragma once
#include <Arduino.h>
#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(SIGMA_BUTTON_EVENT);

typedef enum
{
    SIGMA_BUTTON_EVENT_CLICK,
    SIGMA_BUTTON_EVENT_SHORT_PRESSED,
    SIGMA_BUTTON_EVENT_LONG_PRESSED,
    SIGMA_BUTTON_EVENT_LONG_LONG_PRESSED,
    SIGMA_BUTTON_EVENT_DOUBLE_CLICK,
    SIGMA_BUTTON_EVENT_RELEASED,
    SIGMA_BUTTON_EVENT_SIMPLE_RELEASED,
    SIGMA_BUTTON_EVENT_LONG_RELEASED,
    SIGMA_BUTTON_EVENT_LONG_LONG_RELEASED,
    SIGMA_BUTTON_EVENT_DOUBLE_RELEASED,
    //SIGMA_BUTTON_EVENT_DEBOUNCE,
    SIGMA_BUTTON_EVENT_ERROR
} SigmaButtonEvent;

class SigmaButton
{
public:
    SigmaButton(byte btnId, int pin, bool clickLevel = LOW, bool pullup = false);
    ~SigmaButton();
    void SetDebounceTime(int time) { timeDebounce = time; };
    void SetTimeSimple(int time) { timeSimple = time; };
    void SetTimeLong(int time) { timeLong = time; };
    void SetTimeLongLong(int time) { timeLongLong = time; };
    void SetTimeDouble(int time) { timeDouble = time; };
    int GetDebounceTime() { return timeDebounce; };
    int GetTimeSimple() { return timeSimple; };
    int GetTimeLong() { return timeLong; };
    int GetTimeLongLong() { return timeLongLong; };
    int GetTimeDouble() { return timeDouble; };

    virtual void PinMode(uint8_t pin, uint8_t mode) { pinMode(pin, mode); };
    virtual int DigitalRead(uint8_t pin) { return digitalRead(pin); };

    virtual void AttachInterruptArg(uint8_t pin, void (*func)(void *), void *arg, int mode) { attachInterruptArg(pin, func, arg, mode); };
    virtual void DetachInterrupt(uint8_t pin) { detachInterrupt(pin); };

private:
    byte btnId;
    bool btnClick = false;
    int pin;
    bool clickLevel;
    bool btnStatus = false;
    SigmaButtonEvent btnState = SIGMA_BUTTON_EVENT_ERROR;
    ulong lastClick = 0;

    int timeDebounce = 50;
    int timeSimple = 200;
    int timeLong = 1000;
    int timeLongLong = 3000;
    int timeDouble = 300;

    TimerHandle_t debounceTimer;
    StaticTimer_t debounceTimerBuffer;
    TimerHandle_t cycleTimer;
    StaticTimer_t cycleTimerBuffer;
    TimerHandle_t doubleTimer;
    StaticTimer_t doubleTimerBuffer;

    static void processISR(void *arg);
    void process();
    static void debounceFunc(TimerHandle_t xTimer);
    SigmaButtonEvent releaseEvent();
    static void cycleFunc(TimerHandle_t xTimer);
    static void doubleFunc(TimerHandle_t xTimer);
};
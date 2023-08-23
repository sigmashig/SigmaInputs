#pragma once
#include <Arduino.h>
#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(SIGMABUTTON_EVENT);

typedef enum
{
    SIGMABUTTON_EVENT_CLICK,
    SIGMABUTTON_EVENT_SIMPLE_CLICK,
    SIGMABUTTON_EVENT_LONG_CLICK,
    SIGMABUTTON_EVENT_LONG_LONG_CLICK,
    SIGMABUTTON_EVENT_DOUBLE_CLICK,
    SIGMABUTTON_EVENT_RELEASE,
    SIGMABUTTON_EVENT_SIMPLE_RELEASE,
    SIGMABUTTON_EVENT_LONG_RELEASE,
    SIGMABUTTON_EVENT_LONG_LONG_RELEASE,
    SIGMABUTTON_EVENT_DOUBLE_RELEASE,
    SIGMABUTTON_EVENT_DEBOUNCE,
    SIGMABUTTON_EVENT_ERROR
} SigmaButtonEvent;

class SigmaButton
{
public:
    SigmaButton(int pin, bool clickLevel = LOW, bool pullup = false);
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

private:
    bool btnClick = false;
    int pin;
    bool clickLevel;
    bool btnStatus = false;
    SigmaButtonEvent btnState = SIGMABUTTON_EVENT_ERROR;
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
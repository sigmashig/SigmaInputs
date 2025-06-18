#ifndef SIGMATIMER_H
#define SIGMATIMER_H

#pragma once
#include <Arduino.h>
#include <esp_event.h>
#include <map>

class SigmaTimer
{
public:
    static esp_event_loop_handle_t GetEventLoop();
    static esp_event_base_t GetEventBase()
    {
        init();
        return eventBase;
    }
    static void CreateTimer(unsigned long time);
    static void DeleteTimer(unsigned long time);
    static void DeleteAllTimers();

private:
    inline static esp_event_loop_handle_t eventLoop = nullptr;
    inline static esp_event_base_t eventBase = "SigmaTimerEvents";
    static void init();
    static void timerCallback(TimerHandle_t xTimer);
    inline static std::map<unsigned long, TimerHandle_t> timers;
};

#endif
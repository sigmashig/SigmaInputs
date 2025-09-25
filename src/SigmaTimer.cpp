#include "SigmaTimer.h"

esp_event_loop_handle_t SigmaTimer::GetEventLoop()
{
    init();
    return eventLoop;
}

void SigmaTimer::init()
{
    if (eventLoop == NULL)
    {
        esp_event_loop_args_t loop_args = {
            .queue_size = (int32_t)100,
            .task_name = "SigmaTimer",
            .task_priority = 50,
            .task_stack_size = 4096,
            .task_core_id = 0};

        esp_event_loop_create(&loop_args, (void **)&eventLoop);
        timers.clear();
    }
}

void SigmaTimer::CreateTimer(unsigned long time)
{
    init();
    if (timers.find(time) == timers.end())
    {
        unsigned long t = 0;
        String timerName = "SigmaTimer_" + String(time);
        TimerHandle_t xTimer = xTimerCreate(timerName.c_str(), time / portTICK_PERIOD_MS, pdTRUE, (void *)t, timerCallback);
        timers.insert(std::make_pair(time, xTimer));
        xTimerStart(xTimer, 0);
    }
    else
    { // Nothing to do - timer already exists
    }
}

void SigmaTimer::DeleteTimer(unsigned long time)
{
    if (timers.find(time) != timers.end())
    {
        TimerHandle_t xTimer = timers[time];
        timers.erase(time);
        xTimerDelete(xTimer, 0);
    }
}

void SigmaTimer::DeleteAllTimers()
{
    for (auto &timer : timers)
    {
        xTimerDelete(timer.second, 0);
    }
    timers.clear();
}

void SigmaTimer::timerCallback(TimerHandle_t xTimer)
{
    unsigned long timerCnt = (unsigned long)pvTimerGetTimerID(xTimer);
    unsigned long period = xTimerGetPeriod(xTimer);
    esp_event_post_to(eventLoop, eventBase, timerCnt, (void *)&period, sizeof(period), portMAX_DELAY);
    timerCnt++;
    vTimerSetTimerID(xTimer, (void *)timerCnt);
}

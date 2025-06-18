#include "SigmaTimer.h"

void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    String *s = (String *)arg;
    unsigned long *timerCnt = (unsigned long *)event_data;
    Serial.printf("%s[%d] = time:%d\n", *s, event_id, *timerCnt);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n-------- Hello world! --------");
    String s = "TimerTest";
    esp_event_handler_register_with(SigmaTimer::GetEventLoop(), SigmaTimer::GetEventBase(), ESP_EVENT_ANY_ID, eventHandler, &s);
    SigmaTimer::CreateTimer(1000);
    SigmaTimer::CreateTimer(300);
    delay(5000);
    SigmaTimer::CreateTimer(1000);
    SigmaTimer::CreateTimer(500);
    delay(5000);
    SigmaTimer::DeleteTimer(1000);
    SigmaTimer::DeleteAllTimers();
    delay(1000);
    Serial.println("-------- end --------");
}

void loop()
{
    vTaskDelete(NULL);
}
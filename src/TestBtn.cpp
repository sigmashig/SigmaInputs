#include "SigmaButton.hpp"

SigmaButton *btn;

void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case SIGMABUTTON_EVENT_CLICK:
        Serial.println("Click");
        break;
    case SIGMABUTTON_EVENT_SIMPLE_CLICK:
        Serial.println("Simple click");
        break;
    case SIGMABUTTON_EVENT_DOUBLE_CLICK:
        Serial.println("Double click");
        break;
    case SIGMABUTTON_EVENT_LONG_CLICK:
        Serial.println("Long click");
        break;
    case SIGMABUTTON_EVENT_LONG_LONG_CLICK:
        Serial.println("Long long click");
        break;
    case SIGMABUTTON_EVENT_RELEASE:
        Serial.println("Release");
        break;
    case SIGMABUTTON_EVENT_SIMPLE_RELEASE:
        Serial.println("Simple release");
        break;
    case SIGMABUTTON_EVENT_DOUBLE_RELEASE:
        Serial.println("Double release");
        break;
    case SIGMABUTTON_EVENT_LONG_RELEASE:
        Serial.println("Long release");
        break;
    case SIGMABUTTON_EVENT_LONG_LONG_RELEASE:
        Serial.println("Long long release");
        break;
    case SIGMABUTTON_EVENT_DEBOUNCE:
    {
        bool *btnClick = (bool *)event_data;
        Serial.printf("Debounce=%d\n", *btnClick);
        break;
    }
    case SIGMABUTTON_EVENT_ERROR:
        Serial.println("Error");
        break;
    default:
        Serial.printf("Event: %d\n", event_id);
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("-------- Hello world! --------");
    esp_event_loop_create_default();
    esp_event_handler_register(SIGMABUTTON_EVENT, ESP_EVENT_ANY_ID, eventHandler, NULL);
    Serial.println("-------- Event handler registered --------");
    btn = new SigmaButton(16, LOW, false);
    Serial.println("-------- Button created --------");
}

void loop()
{
    // Serial.println("-------- Loop --------");
    delay(1000);
}
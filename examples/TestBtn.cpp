#include "SigmaButton.hpp"

SigmaButton *btn;

void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    byte *btnId = (byte *)event_data;
    switch (event_id)
    {
    case SIGMA_BUTTON_EVENT_CLICK:
        Serial.printf("Click:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_SIMPLE_CLICK:
        Serial.printf("Simple click:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_DOUBLE_CLICK:
        Serial.printf("Double click:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_LONG_CLICK:
        Serial.printf("Long click:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_LONG_LONG_CLICK:
        Serial.printf("Long long click:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_RELEASE:
        Serial.printf("Release:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_SIMPLE_RELEASE:
        Serial.printf("Simple release:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_DOUBLE_RELEASE:
        Serial.printf("Double release:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_LONG_RELEASE:
        Serial.printf("Long release:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_LONG_LONG_RELEASE:
        Serial.printf("Long long release:%d\n", *btnId);
        break;
    case SIGMA_BUTTON_EVENT_DEBOUNCE:
    {
        Serial.printf("Debounce=%d\n", *btnId);
        break;
    }
    case SIGMA_BUTTON_EVENT_ERROR:
        Serial.printf("Error:%d\n", *btnId);
        break;
    default:
        Serial.printf("Event: %d, id=%d\n", *btnId, event_id);
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("-------- Hello world! --------");
    esp_event_loop_create_default();
    esp_event_handler_register(SIGMA_BUTTON_EVENT, ESP_EVENT_ANY_ID, eventHandler, NULL);
    Serial.println("-------- Event handler registered --------");
    btn = new SigmaButton(111, 16, LOW, false);
    Serial.println("-------- Button created --------");
}

void loop()
{
    // Serial.println("-------- Loop --------");
    delay(1000);
}
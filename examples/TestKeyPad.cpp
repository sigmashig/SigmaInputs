#include <Arduino.h>
#include <SigmaKeypad.hpp>

// ESP_EVENT_DECLARE_BASE(SIGMA_KEYPAD_EVENT);

SigmaKeypad<4, 4> *keypad;

void sigmakeypadEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    char c = *((char *)event_data);

    switch (event_id)
    {
    case SIGMA_KEYPAD_EVENT_KEY_PRESSED:
        Serial.print("SIGMA_KEYPAD_EVENT_KEY_PRESSED: ");
        Serial.println(c);
        break;
    case SIGMA_KEYPAD_EVENT_KEY_RELEASED:
        Serial.print("SIGMA_KEYPAD_EVENT_KEY_RELEASED: ");
        Serial.println(c);
        break;
    }
}
void setup()
{
    Serial.begin(115200);
    Serial.println("Hello World!");

    uint pinRows[4] = {36, 39, 34, 35};
    uint pinColumns[4] = {26, 14, 12, 15};
    const char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};

    esp_event_loop_create_default();

    esp_event_handler_register(SIGMA_KEYPAD_EVENT, ESP_EVENT_ANY_ID, sigmakeypadEventHandler, NULL);
    keypad = new SigmaKeypad<4, 4>(pinRows, pinColumns, keys);
}

void loop()
{
}
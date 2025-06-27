#include "SigmaPS2.h"
#include "SigmaTimer.h"

SigmaPS2 *ps2;

void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    SigmaPS2 *joystick = (SigmaPS2 *)arg;
    if (event_id == SIGMA_RC_DATA_EVENT)
    {
        RcState *psxdata = (RcState *)event_data;

        Serial.printf("Buttons:");
        for (int i = 0; i < 16; i++)
        {
            Serial.printf("%d", psxdata->buttons[i]);
        }
        Serial.printf("\n");
        Serial.printf("Left X: %d|", psxdata->analogJoystick[0].hor);
        Serial.printf("Left Y: %d|", psxdata->analogJoystick[0].vert);
        Serial.printf("Right X: %d|", psxdata->analogJoystick[1].hor);
        Serial.printf("Right Y: %d\n", psxdata->analogJoystick[1].vert);
    }
    else if (event_id == SIGMA_RC_ERROR_EVENT)
    {
        Serial.printf("Error: %s\n", (char *)event_data);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n-------- Hello world! --------");
    String s = "PS2Test";
    RC_PS2_Config rcConfig = {
        .pinCmd = 32,
        .pinClock = 17,
        .pinAck = 33,
        .pinAtt = 16,
        .pinData = 36};


    ps2 = new SigmaPS2("PS2Test", rcConfig);

    esp_event_handler_register_with(ps2->GetEventLoop(), ps2->GetEventBase(), ESP_EVENT_ANY_ID, eventHandler, &s);

    Serial.println("-------- PS2 test end --------");
}

void loop()
{
    /*
    //vTaskDelete(NULL);
    Serial.printf("Loop\n");
    ps2->readData();
    delay(1000);
    Serial.printf("Loop end\n");
    */
}
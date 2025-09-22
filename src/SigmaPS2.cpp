#include "SigmaPS2.h"
#include "SigmaTimer.h"

SigmaPS2::SigmaPS2(String name, RC_PS2_Config rcConfig, esp_event_loop_handle_t loop_handle, esp_event_base_t base)
    : SigmaRemoteControl(name, loop_handle, base)
{
    this->ps2Config = rcConfig;
    setupPins();
    config(PSXMODE_ANALOG);
    //  config(PSXMODE_DIGITAL);

    TimerHandle_t xTimer = xTimerCreate("SigmaPS2", pdMS_TO_TICKS(period), pdTRUE, (void *)this, timerCallback);
    xTimerStart(xTimer, 0);
}

SigmaPS2::~SigmaPS2()
{
}

void SigmaPS2::readData()
{
    PSxData psxData;
    RcState rcState;
    if (read(psxData) == PSXERROR_SUCCESS)
    {
        if (psxData.buttons != lastData.buttons ||
            psxData.JoyLeftX != lastData.JoyLeftX ||
            psxData.JoyLeftY != lastData.JoyLeftY ||
            psxData.JoyRightX != lastData.JoyRightX ||
            psxData.JoyRightY != lastData.JoyRightY)
        {
            lastData = psxData;
            rcState.analogJoystick[0].hor = psxData.JoyLeftX;
            rcState.analogJoystick[0].vert = psxData.JoyLeftY;
            rcState.analogJoystick[1].hor = psxData.JoyRightX;
            rcState.analogJoystick[1].vert = psxData.JoyRightY;
            for (int i = 0; i < 16; i++)
            {
                unsigned int mask = 1 << i;
                rcState.buttons[i] = (psxData.buttons & mask) != 0;
            }
            sendState(rcState);
        }
    }
    else
    {
        sendError("PS2 joystick is not connected");
    }
}
SigmaPS2::PSXButton SigmaPS2::GetPSXButton(String name)
{
    String upperName = name;
    upperName.toUpperCase();
    if (upperName == "PSXBTN_L2") return PSXBTN_L2;
    if (upperName == "PSXBTN_R2") return PSXBTN_R2;
    if (upperName == "PSXBTN_L1") return PSXBTN_L1;
    if (upperName == "PSXBTN_R1") return PSXBTN_R1;
    if (upperName == "PSXBTN_TRIANGLE") return PSXBTN_TRIANGLE;
    if (upperName == "PSXBTN_CIRCLE") return PSXBTN_CIRCLE;
    if (upperName == "PSXBTN_CROSS") return PSXBTN_CROSS;
    if (upperName == "PSXBTN_SQUARE") return PSXBTN_SQUARE;
    if (upperName == "PSXBTN_SELECT") return PSXBTN_SELECT;
    if (upperName == "PSBTN_ACT_LEFT") return PSBTN_ACT_LEFT;
    if (upperName == "PSBTN_ACT_RIGHT") return PSBTN_ACT_RIGHT;
    if (upperName == "PSXBTN_START") return PSXBTN_START;
    if (upperName == "PSXBTN_UP") return PSXBTN_UP;
    if (upperName == "PSXBTN_RIGHT") return PSXBTN_RIGHT;
    if (upperName == "PSXBTN_DOWN") return PSXBTN_DOWN;
    if (upperName == "PSXBTN_LEFT") return PSXBTN_LEFT;
    if (upperName == "PSXJOY_LEFTY") return PSXJOY_LEFTY;
    if (upperName == "PSXJOY_LEFTX") return PSXJOY_LEFTX;
    if (upperName == "PSXJOY_RIGHTY") return PSXJOY_RIGHTY;
    if (upperName == "PSXJOY_RIGHTX") return PSXJOY_RIGHTX;
    return PSXBTN_NONE;
}
String SigmaPS2::PSXButtonToString(uint button)
{
    switch (button)
    {
        case PSXBTN_L2: return "PSXBTN_L2";
        case PSXBTN_R2: return "PSXBTN_R2";
        case PSXBTN_L1: return "PSXBTN_L1";
        case PSXBTN_R1: return "PSXBTN_R1";
        case PSXBTN_TRIANGLE: return "PSXBTN_TRIANGLE";
        case PSXBTN_CIRCLE: return "PSXBTN_CIRCLE";
        case PSXBTN_CROSS: return "PSXBTN_CROSS";
        case PSXBTN_SQUARE: return "PSXBTN_SQUARE";
        case PSXBTN_SELECT: return "PSXBTN_SELECT";
        case PSBTN_ACT_LEFT: return "PSBTN_ACT_LEFT";
        case PSBTN_ACT_RIGHT: return "PSBTN_ACT_RIGHT";
        case PSXBTN_START: return "PSXBTN_START";
        case PSXBTN_UP: return "PSXBTN_UP";
        case PSXBTN_RIGHT: return "PSXBTN_RIGHT";
        case PSXBTN_DOWN: return "PSXBTN_DOWN";
        case PSXBTN_LEFT: return "PSXBTN_LEFT";
        case PSXJOY_LEFTY: return "PSXJOY_LEFTY";
        case PSXJOY_LEFTX: return "PSXJOY_LEFTX";
        case PSXJOY_RIGHTY: return "PSXJOY_RIGHTY";
        case PSXJOY_RIGHTX: return "PSXJOY_RIGHTX";
        default: return "PSXBTN_NONE";
    }
}
void SigmaPS2::readLoop(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    SigmaPS2 *ps2 = (SigmaPS2 *)arg;
    ps2->readData();
}

void SigmaPS2::timerCallback(TimerHandle_t xTimer)
{
    SigmaPS2 *ps2 = (SigmaPS2 *)pvTimerGetTimerID(xTimer);
    ps2->readData();
}

void SigmaPS2::sendCommand(byte command, byte &response)
{
    // Write out a byte command, or read a byte from the controller
    // When reading, the "command" is usually IDLE (0), thus no data gets written
    // The response is always the response from the previous command
    response = 0;
    for (int i = 0; i < 8; i++)
    {
        // Write command bit

        IOError err;
        err = SigmaIO::DigitalWrite(ps2Config.pinCmd, (command & (1 << i)) ? HIGH : LOW);
        if (err != SIGMAIO_SUCCESS)
        {
            Serial.printf("Error writing to pin %d: %d\n", ps2Config.pinCmd, err);
        }
        err = SigmaIO::DigitalWrite(ps2Config.pinClock, LOW);
        if (err != SIGMAIO_SUCCESS)
        {
            Serial.printf("Error writing to pin %d: %d\n", ps2Config.pinClock, err);
        }
        delayMicroseconds(delayStand);
        // Read response bit
        if (SigmaIO::DigitalRead(ps2Config.pinData))
        {
            response |= 1 << i;
        }
        err = SigmaIO::DigitalWrite(ps2Config.pinClock, HIGH);
        if (err != SIGMAIO_SUCCESS)
        {
            Serial.printf("Error writing to pin %d: %d\n", ps2Config.pinClock, err);
        }
        delayMicroseconds(delayStand);
    }
}

void SigmaPS2::setupPins()
{
    // Assign pins
    // Setup pins and pin states
    SigmaIO::PinMode(ps2Config.pinData, INPUT_PULLUP);
    // SigmaIO::DigitalWrite(ps2Config.pinData, HIGH);

    SigmaIO::PinMode(ps2Config.pinCmd, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinCmd, HIGH);

    SigmaIO::PinMode(ps2Config.pinAtt, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);

    SigmaIO::PinMode(ps2Config.pinClock, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinClock, HIGH);

    SigmaIO::PinMode(ps2Config.pinAck, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinAck, HIGH);
}

int SigmaPS2::read(PSxData &psxdata)
{
    // Send data request
    SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
    byte response;
    sendCommand(PSXPROT_HANDSHAKE, response);
    sendCommand(PSXPROT_GETDATA, response);
    sendCommand(PSXPROT_IDLE, response);
    // Check response
    if (response == PSXPROT_STARTDATA)
    {
        // Decode response
        byte data[6];
        for (int i = 0; i < 6; i++)
            sendCommand(PSXPROT_IDLE, data[i]);
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        psxdata.buttons = ~(data[1] + (data[0] << 8));
        psxdata.JoyRightX = stickToDirection(data[2]);
        psxdata.JoyRightY = stickToDirection(data[3]);
        psxdata.JoyLeftX = stickToDirection(data[4]);
        psxdata.JoyLeftY = stickToDirection(data[5]);
        return PSXERROR_SUCCESS;
    }
    else
    {
        //        Serial.printf("No data\n");
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        return PSXERROR_NODATA;
    }
}
int SigmaPS2::stickToDirection(byte x)
{
    int res = 0;

    if (x <= 0x7F)
    {
        res = (100 * (0x7F - x)) / 0x7F;
    }
    else if (x >= 0x81)
    {
        res = -(100 * (x - 0x7F)) / 0x7F;
    }

    return res;
}

void SigmaPS2::config(byte mode)
{
    // Perform initial handshake with the controller
    // Enter config
    byte response = 0;
    for (int j = 0; j < 3; j++)
    { // for unknown reason, the controller needs to be configured several times

        SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
        sendCommand(PSXPROT_HANDSHAKE, response);
        sendCommand(PSXPROT_CONFIG, response);
        sendCommand(PSXPROT_IDLE, response);
        sendCommand(PSXPROT_ENTERCONFIG, response);
        sendCommand(PSXPROT_ZERO, response);
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        delayMicroseconds(delayStand);

        // Set mode
        SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
        sendCommand(PSXPROT_HANDSHAKE, response);
        sendCommand(PSXPROT_CONFIGMODE, response);
        sendCommand(PSXPROT_IDLE, response);
        sendCommand(mode, response);
        sendCommand(PSXPROT_MODELOCK, response);
        for (int i = 0; i < 4; i++)
        {
            sendCommand(PSXPROT_ZERO, response);
        }
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        delayMicroseconds(delayStand);

        // Disable vibration motors
        SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
        sendCommand(PSXPROT_HANDSHAKE, response);
        sendCommand(PSXPROT_CONFIGMOTOR, response);
        sendCommand(PSXPROT_IDLE, response);
        sendCommand(PSXPROT_ZERO, response);
        sendCommand(PSXPROT_MOTORMAP, response);
        for (int i = 0; i < 4; i++)
            sendCommand(PSXPROT_NONZERO, response);
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        delayMicroseconds(delayStand);

        // Finish config
        SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
        sendCommand(PSXPROT_HANDSHAKE, response);
        sendCommand(PSXPROT_CONFIG, response);
        sendCommand(PSXPROT_IDLE, response);
        sendCommand(PSXPROT_EXITCONFIG, response);
        for (int i = 0; i < 5; i++)
            sendCommand(PSXPROT_EXITCFGCNT, response);
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        delay(500);
    }
}
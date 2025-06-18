#include "SigmaPS2.h"
#include "SigmaTimer.h"

SigmaPS2::SigmaPS2(String name, RC_PS2_Config rcConfig, esp_event_loop_handle_t loop_handle, esp_event_base_t base)
  : SigmaRemoteControl(name, loop_handle, base)
{
    this->ps2Config = rcConfig;
    setupPins();
    SigmaTimer::CreateTimer(period);
    esp_event_handler_register_with(SigmaTimer::GetEventLoop(), SigmaTimer::GetEventBase(), period, readLoop, this);
}

SigmaPS2::~SigmaPS2()
{

}

void SigmaPS2::readLoop(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    SigmaPS2 *ps2 = (SigmaPS2 *)arg;
    RcState psxData;
    if (ps2->read(psxData) == PSXERROR_SUCCESS)
    {
        ps2->sendState(psxData);
    }
    else
    {
        ps2->sendError("PS2 joystick is not connected");
    }
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
        SigmaIO::DigitalWrite(ps2Config.pinCmd, (command & (1 << i)) ? HIGH : LOW);
        SigmaIO::DigitalWrite(ps2Config.pinClock, LOW);
        delayMicroseconds(delay);
        // Read response bit
        if (SigmaIO::DigitalRead(ps2Config.pinData))
            response |= 1 << i;
        SigmaIO::DigitalWrite(ps2Config.pinClock, HIGH);
        delayMicroseconds(delay);
    }
}

void SigmaPS2::setupPins()
{
    // Assign pins
    // Setup pins and pin states
    Serial.printf("Set %d\n", ps2Config.pinData);
    SigmaIO::PinMode(ps2Config.pinData, INPUT_PULLUP);
    //SigmaIO::DigitalWrite(ps2Config.pinData, HIGH);

    Serial.printf("Set %d\n", ps2Config.pinCmd);
    SigmaIO::PinMode(ps2Config.pinCmd, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinCmd, HIGH);

    Serial.printf("Set %d\n", ps2Config.pinAtt);
    SigmaIO::PinMode(ps2Config.pinAtt, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);

    Serial.printf("Set %d\n", ps2Config.pinClock);
    SigmaIO::PinMode(ps2Config.pinClock, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinClock, HIGH);

    Serial.printf("Set %d\n", ps2Config.pinAck);
    SigmaIO::PinMode(ps2Config.pinAck, OUTPUT);
    SigmaIO::DigitalWrite(ps2Config.pinAck, HIGH);
}

int SigmaPS2::read(RcState &psxdata)
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
        psxdata.ps2.buttons = ~(data[1] + (data[0] << 8));
        psxdata.ps2.jRigthV = stickToDirection(data[2]);
        psxdata.ps2.jRigthH = stickToDirection(data[3]);
        psxdata.ps2.jLeftV = stickToDirection(data[4]);
        psxdata.ps2.jLeftH = stickToDirection(data[5]);
        return PSXERROR_SUCCESS;
    }
    else
    {
        SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
        return PSXERROR_NODATA;
    }
}
int SigmaPS2::stickToDirection(byte x)
{
    int res = 0;

    if (x <= 0x7F)
    {
        res = -(100 * (0x7F - x)) / 0x7F;
    }
    else if (x >= 0x81)
    {
        res = (100 * (x - 0x7F)) / 0x7F;
    }

    return res;
}

void SigmaPS2::config(byte mode)
{
    // Perform initial handshake with the controller
    // Enter config
    byte response;
    SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
    sendCommand(PSXPROT_HANDSHAKE, response);
    sendCommand(PSXPROT_CONFIG, response);
    sendCommand(PSXPROT_IDLE, response);
    sendCommand(PSXPROT_ENTERCONFIG, response);
    sendCommand(PSXPROT_ZERO, response);
    SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
    delayMicroseconds(delay);
    // Set mode
    SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
    sendCommand(PSXPROT_HANDSHAKE, response);
    sendCommand(PSXPROT_CONFIGMODE, response);
    sendCommand(PSXPROT_IDLE, response);
    sendCommand(mode, response);
    sendCommand(PSXPROT_MODELOCK, response);
    for (int i = 0; i < 4; i++)
        sendCommand(PSXPROT_ZERO, response);
    SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
    delayMicroseconds(delay);
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
    delayMicroseconds(delay);
    // Finish config
    SigmaIO::DigitalWrite(ps2Config.pinAtt, LOW);
    sendCommand(PSXPROT_HANDSHAKE, response);
    sendCommand(PSXPROT_CONFIG, response);
    sendCommand(PSXPROT_IDLE, response);
    sendCommand(PSXPROT_EXITCONFIG, response);
    for (int i = 0; i < 5; i++)
        sendCommand(PSXPROT_EXITCFGCNT, response);
    SigmaIO::DigitalWrite(ps2Config.pinAtt, HIGH);
}


#ifndef SIGMAPS2_H
#define SIGMAPS2_H

#pragma once

#include <Arduino.h>
#include <SigmaIO.h>
#include <SigmaRemoteControl.h>

// Button bitfield constants
/*
#define PSXBTN_DOWN 0x4000
#define PSXBTN_RIGHT 0x2000
#define PSXBTN_UP 0x1000
#define PSXBTN_START 0x0800
#define PSBTN_ACT_RIGHT 0x0400
#define PSBTN_ACT_LEFT 0x0200
#define PSXBTN_SELECT 0x0100
#define PSXBTN_SQUARE 0x0080
#define PSXBTN_CROSS 0x0040
#define PSXBTN_CIRCLE 0x0020
#define PSXBTN_TRIANGLE 0x0010
#define PSXBTN_R1 0x0008
#define PSXBTN_L1 0x0004
#define PSXBTN_R2 0x0002
#define PSXBTN_L2 0x0001
*/

// Protocol commands and constants

#define PSXPROT_IDLE 0x00
#define PSXPROT_NONZERO 0xFF
#define PSXPROT_ZERO 0x00
#define PSXPROT_HANDSHAKE 0x01
#define PSXPROT_GETDATA 0x42
#define PSXPROT_STARTDATA 0x5A
#define PSXPROT_CONFIG 0x43
#define PSXPROT_CONFIGMOTOR 0x4D
#define PSXPROT_MOTORMAP 0x01
#define PSXPROT_ENTERCONFIG 0x01
#define PSXPROT_EXITCONFIG 0x00
#define PSXPROT_EXITCFGCNT 0x5A
#define PSXPROT_CONFIGMODE 0x44
#define PSXPROT_MODELOCK 0x03
#define PSXMODE_ANALOG 0x01
#define PSXMODE_DIGITAL 0x00

// Read function return codes

#define PSXERROR_SUCCESS 0
#define PSXERROR_NODATA 1

#endif

typedef struct
{
    unsigned int buttons; //!< The state of all buttons encoded as a bitfield
    int JoyLeftX;         //!< The horizontal value of the left joystick in persent
    int JoyLeftY;         //!< The vertical value of the left joystick in persent
    int JoyRightX;        //!< The horizontal value of the right joystick in persent
    int JoyRightY;        //!< The vertical value of the right joystick in persent
} PSxData;

class SigmaPS2 : public SigmaRemoteControl
{
    /* Green: ACK
     * Blue: Clock
     * Yellow: Att
     * Red: Power
     * Black: GND
     * Grey: 7-9V for vibro
     * Orange: CMD
     * Brown: Data
     * Data and ACK should be pulled-Up to power with 1-1kOhm
     */
public:
    SigmaPS2(String name, RC_PS2_Config rcConfig, esp_event_loop_handle_t loop_handle = nullptr, esp_event_base_t base = nullptr);
    ~SigmaPS2();
    void readData();

private:
    RC_PS2_Config ps2Config;
    const int delayStand = 10;
    const uint period = 100;
    PSxData lastData = {0};
    /**
     * @brief Read the state of all joysticks and buttons from the controller
     *
     * @param psxdata The state read from the controller
     * @return 0 - success, 1 - error
     */
    int read(PSxData &psxdata);
    int stickToDirection(byte x);
    /**
     * @brief Configures the controller, disables vibration and sets either analog or digital mode
     *
     * @param mode What mode to set. PSXMODE_ANALOG or PSXMODE_DIGITAL.
     */
    void config(byte mode);
    void sendCommand(byte command, byte &response);
    void setupPins();
    static void readLoop(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void timerCallback(TimerHandle_t xTimer);

};

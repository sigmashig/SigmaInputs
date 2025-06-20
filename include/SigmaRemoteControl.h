#ifndef SIGMAREMOTECONTROL_H
#define SIGMAREMOTECONTROL_H

#pragma once
#include <Arduino.h>
#include <esp_event.h>

typedef enum
{
    SIGMA_RC_PS2
} RcTypes;

typedef struct
{
    uint pinCmd;
    uint pinClock;
    uint pinAck;
    uint pinAtt;
    uint pinData;
} RC_PS2_Config;

typedef struct
{
    String name;
    RcTypes type;
    
    union
    {
        RC_PS2_Config ps2;
    } params;
} RcConfig;

typedef union
{
    struct
    {
        int jLeftV;
        int jLeftH;
        int jRigthV;
        int jRigthH;
        u16_t buttons;
    } ps2;
} RcState;
typedef enum
{
    BTN_L2 = 0x0001,
    BTN_R2 = 0x0002,
    BTN_L1 = 0x0004,
    BTN_R1 = 0x0008,
    BTN_TRIANGLE = 0x0010,
    BTN_CIRCLE = 0x0020,
    BTN_CROSS = 0x0040,
    BTN_SQUARE = 0x0080,
    BTN_SELECT = 0x0100,
    BTN_ACT_LEFT = 0x0200,
    BTN_ACT_RIGHT = 0x0400,
    BTN_START = 0x0800,
    BTN_UP = 0x1000,
    BTN_RIGHT = 0x2000,
    BTN_DOWN = 0x4000,
    BTN_LEFT = 0x8000
} RcButton;

typedef enum {
    SIGMA_RC_DATA_EVENT,
    SIGMA_RC_ERROR_EVENT,
} RcEvent;

class SigmaRemoteControl
{
public:
    SigmaRemoteControl(String name, esp_event_loop_handle_t loop_handle = nullptr, esp_event_base_t base = nullptr);
    ~SigmaRemoteControl();
    esp_event_loop_handle_t GetEventLoop() const { return event_loop_handle; }
    esp_event_base_t GetEventBase() const { return event_base; }
    static RcTypes GetRcType(String name);
    static SigmaRemoteControl *Create(RcConfig rcConfig);

protected:
    void sendState(RcState state);
    void sendError(String error);

    esp_event_loop_handle_t event_loop_handle;
    esp_event_base_t event_base;
    String name;
    //   RcConfig rcConfig;

private:
};

#endif
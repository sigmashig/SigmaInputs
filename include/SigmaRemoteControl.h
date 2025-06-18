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
        int16_t buttons;
    } ps2;
} RcState;

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
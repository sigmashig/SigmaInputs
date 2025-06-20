#include "SigmaRemoteControl.h"
#include "SigmaPS2.h"

SigmaRemoteControl::SigmaRemoteControl(String name, esp_event_loop_handle_t loop_handle, esp_event_base_t event_base)
{
    this->name = name;
    // this->rcConfig = rcConfig;
    if (loop_handle == nullptr)
    {
        esp_event_loop_args_t loop_args = {
            .queue_size = (int32_t)100,
            .task_name = name.c_str(),
            .task_priority = 50,
            .task_stack_size = 4096,
            .task_core_id = 0};
        esp_event_loop_create(&loop_args, (void **)&event_loop_handle);
    }
    else
    {
        this->event_loop_handle = loop_handle;
    }
    if (event_base == nullptr)
    {
        this->event_base = name.c_str();
    }
    else
    {
        this->event_base = event_base;
    }
}

SigmaRemoteControl::~SigmaRemoteControl()
{
}

RcTypes SigmaRemoteControl::GetRcType(String name)
{
    if (name == "PSX")
    {
        return SIGMA_RC_PS2;
    }
    return SIGMA_RC_PS2;
}

void SigmaRemoteControl::sendError(String error)
{
    esp_event_post_to(event_loop_handle, event_base, SIGMA_RC_ERROR_EVENT, (void *)error.c_str(), error.length() + 1, portMAX_DELAY);
}

SigmaRemoteControl *SigmaRemoteControl::Create(RcConfig rcConfig)
{
    switch (rcConfig.type)
    {
    case SIGMA_RC_PS2:
        return new SigmaPS2(rcConfig.name, rcConfig.params.ps2);
    default:
        return nullptr;
    }
    return nullptr;
}

void SigmaRemoteControl::sendState(RcState state)
{
    esp_event_post_to(event_loop_handle, event_base, SIGMA_RC_DATA_EVENT, (void *)&state, sizeof(RcState), portMAX_DELAY);
}
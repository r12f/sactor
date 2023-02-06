#pragma once

#include "sactor/sactor.h"
#include "driver/gpio.h"

DECLARE_MESSAGE_ID_BEGIN(ActorSchedulerImpl)
    MESSAGE_ID(ActorSchedulerImpl, ScheduleTimerMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_ID_BEGIN(ActorLedCtrlImpl)
    MESSAGE_ID(ActorLedCtrlImpl, ControlMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_BEGIN(ActorLedCtrlImpl, ControlMessage)
    gpio_num_t Pin;
    bool IsOn;
DECLARE_MESSAGE_END()
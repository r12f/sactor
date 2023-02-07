#pragma once

#include "sactor.h"
#include "driver/gpio.h"

DECLARE_MESSAGE_ID_BEGIN(ActorLedCtrlImpl)
    MESSAGE_ID(ActorLedCtrlImpl, ControlMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_BEGIN(ActorLedCtrlImpl, ControlMessage)
    gpio_num_t pin;
    bool is_on;
DECLARE_MESSAGE_END()
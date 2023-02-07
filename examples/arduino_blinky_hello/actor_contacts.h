#pragma once

#include "common.h"

DECLARE_MESSAGE_ID_BEGIN(ActorHelloImpl)
    DECLARE_MESSAGE_ID(ActorHelloImpl, HelloMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_BEGIN(ActorHelloImpl, HelloMessage)
    bool is_on;
DECLARE_MESSAGE_END()
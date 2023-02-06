#pragma once

#include "sactor/sactor.h"

DECLARE_MESSAGE_ID_BEGIN(ActorHello)
    MESSAGE_ID(ActorHello, HelloMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_BEGIN(ActorHello, HelloMessage)
    bool IsOn;
DECLARE_MESSAGE_END()
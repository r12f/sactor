#pragma once

#include <stdio.h>
#include "sactor.h"
#include "actor_contacts.h"

class ActorSchedulerImpl : public ActorImpl
{
public:
    static constexpr const char* NAME = "ActorScheduler";

    MESSAGE_MAP_BEGIN()
        ON_INIT_MESSAGE(ActorSchedulerImpl, on_init)
    MESSAGE_MAP_END()

private:
    SactorError on_init();
};

class ActorLedCtrlImpl : public ActorImpl
{
public:
    static constexpr const char* NAME = "ActorLedCtrl";

    MESSAGE_MAP_BEGIN()
        ON_MESSAGE_NO_REPLY(ActorLedCtrlImpl, on_control, ControlMessage)
    MESSAGE_MAP_END()

private:
    SactorError on_control(_In_ const ControlMessage* message);
};

typedef Actor<ActorSchedulerImpl> ActorScheduler;
typedef PooledActor<ActorLedCtrlImpl> ActorLedCtrl;

extern ActorScheduler scheduler;
extern ActorMailbox actor_pool_mailbox;
extern ActorLedCtrl led_ctrl_0;
extern ActorLedCtrl led_ctrl_1;
extern ActorLedCtrl led_ctrl_2;
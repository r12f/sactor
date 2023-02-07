#pragma once

#include <stdio.h>
#include "sactor.h"
#include "actor_contacts.h"

class ActorSchedulerImpl : public ActorImpl
{
    bool isOn_;

public:
    static constexpr const char* NAME = "ActorScheduler";

    ActorSchedulerImpl() : isOn_(true) {}

    MESSAGE_MAP_BEGIN()
        ON_INIT_MESSAGE(ActorSchedulerImpl, on_init)
        ON_MESSAGE_NO_PAYLOAD(ActorSchedulerImpl, on_schedule_timer, ScheduleTimerMessage)
    MESSAGE_MAP_END()

private:
    SactorError on_init();
    SactorError on_schedule_timer();
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
extern ActorLedCtrl ledCtrl;
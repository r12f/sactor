#pragma once

#include <stdio.h>
#include "sactor/sactor.h"
#include "actor_contacts.h"

class ActorSchedulerImpl : public ActorImpl
{
public:
    static constexpr const char* Name = "ActorScheduler";
    static constexpr uint32_t StackWordCount = 2048;

    MESSAGE_MAP_BEGIN()
        ON_INIT_MESSAGE(ActorSchedulerImpl, OnInit)
    MESSAGE_MAP_END()

private:
    SactorError OnInit();
};

class ActorLedCtrlImpl : public ActorImpl
{
public:
    static constexpr const char* Name = "ActorLedCtrl";

    MESSAGE_MAP_BEGIN()
        ON_MESSAGE_NO_REPLY(ActorLedCtrlImpl, OnControl, ControlMessage)
    MESSAGE_MAP_END()

private:
    SactorError OnControl(_In_ const ControlMessage* message);
};

typedef Actor<ActorSchedulerImpl> ActorScheduler;
typedef PooledActor<ActorLedCtrlImpl> ActorLedCtrl;

extern ActorScheduler scheduler;
extern ActorMailbox actorPoolMailbox;
extern ActorLedCtrl ledCtrl0;
extern ActorLedCtrl ledCtrl1;
extern ActorLedCtrl ledCtrl2;
#pragma once

#include "actor_contacts.h"

class ActorHelloImpl : public ActorImpl
{
public:
    static constexpr const char* Name = "ActorHello";

    MESSAGE_MAP_BEGIN()
        ON_MESSAGE_NO_REPLY(ActorHelloImpl, OnHello, HelloMessage)
    MESSAGE_MAP_END()

private:
    SactorError OnHello(_In_ const HelloMessage* message) {
        printf("Hello world! LED = %s\n", message->IsOn ? "On" : "Off");
        return SactorError_NoError;
    }
};

class ActorBlinkyImpl : public ActorImpl
{
public:
    static constexpr const char* Name = "ActorBlinky";

    MESSAGE_MAP_BEGIN()
        ON_INIT_MESSAGE(ActorBlinkyImpl, OnInit)
    MESSAGE_MAP_END()

private:
    SactorError OnInit();
};

typedef Actor<ActorHelloImpl> ActorHello;
typedef Actor<ActorBlinkyImpl> ActorBlinky;

extern ActorHello hello;
extern ActorBlinky blinky;


#pragma once

#include "actor_contacts.h"

class ActorHelloImpl : public ActorImpl
{
public:
    static constexpr const char* NAME = "ActorHello";

    MESSAGE_MAP_BEGIN()
        ON_MESSAGE_NO_REPLY(ActorHelloImpl, on_hello, HelloMessage)
    MESSAGE_MAP_END()

private:
    SactorError on_hello(_In_ const HelloMessage* message) {
        printf("Hello world! LED = %s\n", message->IsOn ? "On" : "Off");
        return SactorError_NoError;
    }
};

class ActorBlinkyImpl : public ActorImpl
{
public:
    static constexpr const char* NAME = "ActorBlinky";

    MESSAGE_MAP_BEGIN()
        ON_INIT_MESSAGE(ActorBlinkyImpl, on_init)
    MESSAGE_MAP_END()

private:
    SactorError on_init();
};

typedef Actor<ActorHelloImpl> ActorHello;
typedef Actor<ActorBlinkyImpl> ActorBlinky;

extern ActorHello hello;
extern ActorBlinky blinky;


#pragma once

#include <stdio.h>
#include <sactor.h>
#include <Arduino.h>

#define BLINK_GPIO 10

DECLARE_MESSAGE_ID_BEGIN(ActorHelloImpl)
    DECLARE_MESSAGE_ID(ActorHelloImpl, HelloMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_BEGIN(ActorHelloImpl, HelloMessage)
    bool IsOn;
DECLARE_MESSAGE_END()

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

ActorHello hello;
ActorBlinky blinky;

SactorError ActorBlinkyImpl::OnInit()
{
    pinMode(BLINK_GPIO, OUTPUT);

    while(1) {
        /* Blink off (output low) */
        printf("Turning off the LED\n");
        digitalWrite(BLINK_GPIO, LOW);
        hello.SendSync(HelloMessage { false });
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        /* Blink on (output high) */
        printf("Turning on the LED\n");
        digitalWrite(BLINK_GPIO, HIGH);
        hello.SendSync(HelloMessage { true });
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup()
{
  Serial.begin(9600);

  hello.Start();
  blinky.Start();
}

void loop()
{
}

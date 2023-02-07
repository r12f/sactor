#include "actors.h"

ActorHello hello;
ActorBlinky blinky;

#ifndef LED_BUILTIN
#define LED_BUILTIN 10
#endif

SactorError ActorBlinkyImpl::on_init()
{
    pinMode(LED_BUILTIN, OUTPUT);

    while(1) {
        /* Blink off (output low) */
        printf("Turning off the LED\n");
        digitalWrite(LED_BUILTIN, LOW);
        hello.send_sync(HelloMessage { false });
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        /* Blink on (output high) */
        printf("Turning on the LED\n");
        digitalWrite(LED_BUILTIN, HIGH);
        hello.send_sync(HelloMessage { true });
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
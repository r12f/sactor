#include "actors.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO GPIO_NUM_10

ActorHello hello;
ActorBlinky blinky;

SactorError ActorHelloImpl::on_hello(_In_ const HelloMessage* message)
{
    printf("Hello world! LED = %s\n", message->is_on ? "On" : "Off");
    return SactorError_NoError;
}

SactorError ActorBlinkyImpl::on_init()
{
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        hello.send_sync(HelloMessage { false });
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        /* Blink on (output high) */
        printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        hello.send_sync(HelloMessage { true });
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
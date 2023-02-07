#include "actors.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED0_GPIO GPIO_NUM_10
#define LED1_GPIO GPIO_NUM_11
#define LED2_GPIO GPIO_NUM_12

ActorScheduler scheduler;

uint8_t actorPoolMailboxQueueBuffer[ActorMailbox::ItemSize * 100];
ActorMailbox actor_pool_mailbox { "LedCtrl*", actorPoolMailboxQueueBuffer, 100 };
ActorLedCtrl led_ctrl_0 { actor_pool_mailbox };
ActorLedCtrl led_ctrl_1 { actor_pool_mailbox };
ActorLedCtrl led_ctrl_2 { actor_pool_mailbox };

SactorError ActorSchedulerImpl::on_init()
{
    static gpio_num_t ledPins[3] = { LED0_GPIO, LED1_GPIO, LED2_GPIO };

    for (gpio_num_t ledPin : ledPins) {
        esp_rom_gpio_pad_select_gpio(ledPin);
        gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);
    }

    bool isOn = true;

    while(1) {
        printf("Schedule LED status update: Status = %s\n", isOn ? "On" : "Off");

        for (gpio_num_t ledPin : ledPins) {
            actor_pool_mailbox.tx().send_sync(ControlMessage { ledPin, isOn });
        }
        isOn = !isOn;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

SactorError ActorLedCtrlImpl::on_control(_In_ const ControlMessage* message)
{
    printf("Updating LED Status: Actor = %p, Pin = %d, Status = %s\n", this, message->Pin, message->IsOn ? "On" : "Off");
    gpio_set_level(message->Pin, message->IsOn ? 1 : 0);
    return SactorError_NoError;
}
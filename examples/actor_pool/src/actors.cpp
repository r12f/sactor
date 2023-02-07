#include "actors.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED0_GPIO GPIO_NUM_10
#define LED1_GPIO GPIO_NUM_11
#define LED2_GPIO GPIO_NUM_12

ActorScheduler scheduler;

uint8_t actor_pool_mailbox_queue_buffer[ActorMailbox::ItemSize * 100];
ActorMailbox actor_pool_mailbox { "LedCtrl*", actor_pool_mailbox_queue_buffer, 100 };
ActorLedCtrl led_ctrl_0 { actor_pool_mailbox };
ActorLedCtrl led_ctrl_1 { actor_pool_mailbox };
ActorLedCtrl led_ctrl_2 { actor_pool_mailbox };

SactorError ActorSchedulerImpl::on_init()
{
    static gpio_num_t led_pins[3] = { LED0_GPIO, LED1_GPIO, LED2_GPIO };

    for (gpio_num_t led_pin : led_pins) {
        esp_rom_gpio_pad_select_gpio(led_pin);
        gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);
    }

    bool is_on = true;

    while(1) {
        printf("Schedule LED status update: Status = %s\n", is_on ? "On" : "Off");

        for (gpio_num_t led_pin : led_pins) {
            actor_pool_mailbox.tx().send_sync(ControlMessage { led_pin, is_on });
        }
        is_on = !is_on;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

SactorError ActorLedCtrlImpl::on_control(_In_ const ControlMessage* message)
{
    printf("Updating LED Status: Actor = %p, Pin = %d, Status = %s\n", this, message->pin, message->is_on ? "On" : "Off");
    gpio_set_level(message->pin, message->is_on ? 1 : 0);
    return SactorError_NoError;
}
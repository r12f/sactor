#include "actors.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED_GPIO GPIO_NUM_10

ActorScheduler scheduler;

uint8_t actor_pool_mailbox_queue_buffer[ActorMailbox::ItemSize * 100];
ActorMailbox actor_pool_mailbox { "LedCtrl*", actor_pool_mailbox_queue_buffer, 100 };
ActorLedCtrl led_ctrl { actor_pool_mailbox };

SactorError ActorSchedulerImpl::on_init()
{
    esp_rom_gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    queue_delayed_message(MESSAGE_ID(ActorSchedulerImpl, ScheduleTimerMessage), 1000);
    return SactorError_NoError;
}

SactorError ActorSchedulerImpl::on_schedule_timer()
{
    printf("Schedule LED status update: Status = %s\n", is_on_ ? "On" : "Off");
    actor_pool_mailbox.tx().send_sync(ControlMessage { LED_GPIO, is_on_ });
    is_on_ = !is_on_;

    queue_delayed_message(MESSAGE_ID(ActorSchedulerImpl, ScheduleTimerMessage), 1000);
    return SactorError_NoError;
}

SactorError ActorLedCtrlImpl::on_control(_In_ const ControlMessage* message)
{
    printf("Updating LED Status: Actor = %p, Pin = %d, Status = %s\n", this, message->pin, message->is_on ? "On" : "Off");
    gpio_set_level(message->pin, message->is_on ? 1 : 0);
    return SactorError_NoError;
}
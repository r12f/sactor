#include "actors.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED_GPIO GPIO_NUM_10

ActorScheduler scheduler;

uint8_t actorPoolMailboxQueueBuffer[ActorMailbox::ItemSize * 100];
ActorMailbox actorPoolMailbox { "LedCtrl*", actorPoolMailboxQueueBuffer, 100 };
ActorLedCtrl ledCtrl { actorPoolMailbox };

SactorError ActorSchedulerImpl::OnInit()
{
    esp_rom_gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    queue_delayed_message(MESSAGE_ID(ActorSchedulerImpl, ScheduleTimerMessage), 1000);
    return SactorError_NoError;
}

SactorError ActorSchedulerImpl::OnScheduleTimer()
{
    printf("Schedule LED status update: Status = %s\n", isOn_ ? "On" : "Off");
    actorPoolMailbox.tx().send_sync(ControlMessage { LED_GPIO, isOn_ });
    isOn_ = !isOn_;

    queue_delayed_message(MESSAGE_ID(ActorSchedulerImpl, ScheduleTimerMessage), 1000);
    return SactorError_NoError;
}

SactorError ActorLedCtrlImpl::OnControl(_In_ const ControlMessage* message)
{
    printf("Updating LED Status: Actor = %p, Pin = %d, Status = %s\n", this, message->Pin, message->IsOn ? "On" : "Off");
    gpio_set_level(message->Pin, message->IsOn ? 1 : 0);
    return SactorError_NoError;
}
#include "sactor/actor.h"
#include "sactor/delay_message_service.h"

static DelayMessageService dms;

SactorError ActorImpl::queue_delayed_message(_In_ BaseType_t message_id, _In_ uint32_t delay_in_ms)
{
    // This is ensured by ActorCommon ctor.
    SACTOR_REQUIRES(mailbox_ != nullptr);
    return dms.queue_delayed_message(*mailbox_, message_id, delay_in_ms);
}

SactorError ActorImpl::ProcessIncomingMessage(_In_ BaseType_t message_id, _In_opt_ const void* message, _In_opt_ void* message_reply)
{
    switch (message_id)
    {
    case MESSAGE_ID_INIT:
        return on_init();
    case MESSAGE_ID_QUIT:
        on_quit();
        return SactorError_NoError;
    default:
        break;
    }

    return ProcessCustomerIncomingMessage(message_id, message, message_reply);
}

SactorError ActorImpl::ProcessCustomerIncomingMessage(_In_ BaseType_t message_id, _In_opt_ const void* message, _In_opt_ void* message_reply)
{
    return SactorError_NoError;
}

SactorError ActorImpl::on_init()
{
    return SactorError_NoError;
}

void ActorImpl::on_quit()
{
    return;
}

SactorError PollingActorImpl::ProcessIncomingMessage(_In_ BaseType_t message_id, _In_opt_ const void* message, _In_opt_ void* message_reply)
{
    switch (message_id)
    {
    case MESSAGE_ID_POLLING_TIMER:
        on_polling_timer();
        schedule_polling_timer();
        return SactorError_NoError;
    default:
        break;
    }

    return ActorImpl::ProcessIncomingMessage(message_id, message, message_reply);
}

PollingActorImpl::PollingActorImpl(_In_ uint32_t polling_interval_in_ms)
    : polling_interval_in_ms_(polling_interval_in_ms)
{
}

SactorError PollingActorImpl::on_init()
{
    schedule_polling_timer();
    return ActorImpl::on_init();
}

void PollingActorImpl::on_polling_timer()
{
}

void PollingActorImpl::schedule_polling_timer()
{
    SACTOR_ENSURES(queue_delayed_message(MESSAGE_ID_POLLING_TIMER, polling_interval_in_ms_) == SactorError_NoError);
}
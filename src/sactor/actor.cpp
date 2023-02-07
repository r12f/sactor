#include "sactor/actor.h"
#include "sactor/delay_message_service.h"

static DelayMessageService dms;

SactorError ActorImpl::queue_delayed_message(_In_ BaseType_t message_id, _In_ uint32_t delay_in_ms)
{
    // This is ensured by ActorCommon ctor.
    SACTOR_REQUIRES(mailbox_ != nullptr);
    return dms.queue_delayed_message(*mailbox_, message_id, delay_in_ms);
}

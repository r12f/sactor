#include "sactor/actor.h"
#include "sactor/delay_message_service.h"

static DelayMessageService dms;

SactorError ActorImpl::QueueDelayedMessage(_In_ BaseType_t message_id, _In_ uint32_t delay_in_ms)
{
    // This is ensured by ActorCommon ctor.
    SACTOR_REQUIRES(mailbox_ != nullptr);
    return dms.QueueDelayedMessage(*mailbox_, message_id, delay_in_ms);
}

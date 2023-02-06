#include "sactor/actor.h"
#include "sactor/delay_message_service.h"

static DelayMessageService dms;

SactorError ActorImpl::QueueDelayedMessage(_In_ BaseType_t messageId, _In_ uint32_t delayInMs)
{
    // This is ensured by ActorCommon ctor.
    SACTOR_REQUIRES(mailbox_ != nullptr);
    return dms.QueueDelayedMessage(*mailbox_, messageId, delayInMs);
}
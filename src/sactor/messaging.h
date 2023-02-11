#ifndef SACTOR_MESSAGING_H
#define SACTOR_MESSAGING_H

#include "sactor/common.h"

// This can be used to help define your own internal message id range.
#ifndef SACTOR_ACTOR_MESSAGE_ID_BASE
#define SACTOR_ACTOR_MESSAGE_ID_BASE 0
#endif

#define DECLARE_MESSAGE_ID_BEGIN(ActorType) \
    enum ActorMessageId_ ## ActorType { \
        ActorType ## _MessageId_Base = SACTOR_ACTOR_MESSAGE_ID_BASE,

#define MESSAGE_ID(ActorType, MessageType) \
    MessageId_ ## ActorType ## _ ## MessageType

#define DECLARE_MESSAGE_ID(ActorType, MessageType) \
    MESSAGE_ID(ActorType, MessageType),

#define DECLARE_MESSAGE_ID_SECTION_START(ActorType, MessageType, MessageId) \
    MESSAGE_ID(ActorType, MessageType) = MessageId,

#define DECLARE_MESSAGE_ID_END() \
    };

#define DECLARE_MESSAGE_BEGIN(ActorType, MessageType) \
    struct MessageType { \
        static const int ID = MessageId_ ## ActorType ## _ ## MessageType;

#define DECLARE_MESSAGE_END() \
    };

#define MESSAGE_MAP_BEGIN() \
    SactorError ProcessCustomerIncomingMessage(_In_ BaseType_t message_id, _In_opt_ const void* message, _In_opt_ void* message_reply) override { \
        switch (message_id) {

#define ON_MESSAGE(ActorType, HandlerFunc, MessageType, MessageReplyType) \
        case MESSAGE_ID(ActorType, MessageType): \
            return this->HandlerFunc((const MessageType*)message, (MessageReplyType*)message_reply);

#define ON_MESSAGE_NO_REPLY(ActorType, HandlerFunc, MessageType) \
        case MESSAGE_ID(ActorType, MessageType): \
            return this->HandlerFunc((const MessageType*)message);

#define ON_MESSAGE_NO_PAYLOAD(ActorType, HandlerFunc, MessageType) \
        case MESSAGE_ID(ActorType, MessageType): \
            return this->HandlerFunc();

#define ON_MESSAGE_CUSTOM_PAYLOAD(ActorType, MessageType, HandlerFunc, MessaagePayloadType, MessageReplyType) \
        case MESSAGE_ID(ActorType, MessageType): \
            return this->HandlerFunc((const MessaagePayloadType*)message, (MessageReplyType*)message_reply);

#define MESSAGE_MAP_END() \
        } \
        return SactorError_NoError; \
    }

//
// Inbox message IDs
//
#define MESSAGE_ID_INIT (-1)
#define MESSAGE_ID_QUIT (-2)
#define MESSAGE_ID_POLLING_TIMER (-3)

#endif

#ifndef SACTOR_MESSAGING_H
#define SACTOR_MESSAGING_H

#include "common.h"

#define DECLARE_MESSAGE_ID_BEGIN(ActorType) \
    enum ActorMessageId_ ## ActorType { \
        ActorType ## _MessageId_Base = 0,

#define MESSAGE_ID(ActorType, MessageType) \
    MessageId_ ## ActorType ## _ ## MessageType

#define DECLARE_MESSAGE_ID(ActorType, MessageType) \
    MESSAGE_ID(ActorType, MessageType),

#define DECLARE_MESSAGE_ID_END() \
    };

#define DECLARE_MESSAGE_BEGIN(ActorType, MessageType) \
    struct MessageType { \
        static const int Id = MessageId_ ## ActorType ## _ ## MessageType;

#define DECLARE_MESSAGE_END() \
    };

#define MESSAGE_MAP_BEGIN() \
    SactorError ProcessIncomingMessage(_In_ BaseType_t messageId, _In_opt_ const void* message, _In_opt_ void* messageReply) { \
        switch (messageId) {

#define ON_INIT_MESSAGE(ActorType, HandlerFunc) \
        case MESSAGE_ID_INIT: \
            return this->HandlerFunc();

#define ON_QUIT_MESSAGE(ActorType, HandlerFunc) \
        case MESSAGE_ID_QUIT: \
            return this->HandlerFunc();

#define ON_MESSAGE(ActorType, HandlerFunc, MessageType, MessageReplyType) \
        case MESSAGE_ID(ActorType, MessageType): \
            return this->HandlerFunc((const MessageType*)message), (MessageReplyType*)messageReply);

#define ON_MESSAGE_NO_REPLY(ActorType, HandlerFunc, MessageType) \
        case MESSAGE_ID(ActorType, MessageType): \
            return this->HandlerFunc((const MessageType*)message);

#define MESSAGE_MAP_END() \
        } \
        return SactorError_NoError; \
    }

//
// Inbox message IDs
//
#define MESSAGE_ID_INIT (-1)
#define MESSAGE_INIT()  \
    MESSAGE_ID_INIT, nullptr, nullptr

#define MESSAGE_ID_QUIT (-2)
#define MESSAGE_QUIT()  \
    MESSAGE_ID_QUIT, nullptr, nullptr

#endif
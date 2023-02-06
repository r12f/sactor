#ifndef SACTOR_ACTOR_H
#define SACTOR_ACTOR_H

#include "sactor/common.h"
#include "sactor/actor_mailbox.h"
#include "sactor/actor_worker_thread.h"

template <class T>
class ActorWorkerThread;

template <class T>
class ActorCommon
{
    T impl_;
    ActorMailbox& mailbox_;
    ActorWorkerThread<T> worker_;

public:
    void Start() {
        SACTOR_TRACE_ACTOR_START(T::Name, this);
        worker_.Start();
        SACTOR_TRACE_ACTOR_STARTED(T::Name, this);
    }

    template <class MessageType>
    SactorError SendSync(_In_ const MessageType& message) {
        return mailbox_.Tx().SendSync(message);
    }

    template <class MessageType, class ReplyType>
    SactorError SendRecvSync(_In_ const MessageType& message, _Out_ ReplyType& reply) {
        return mailbox_.Tx().SendRecvSync(message, reply);
    }

    SactorError SendAsync(_In_ BaseType_t messageId) {
        return mailbox_.Tx().SendAsync(messageId);
    }

protected:
    ActorCommon(_In_ ActorMailbox& mailbox)
        : mailbox_(mailbox)
        , worker_(mailbox, impl_)
    {
        SACTOR_TRACE_ACTOR_CREATED(T::Name, this);
    }
};

template <class T>
class Actor : public ActorCommon<T>
{
    ActorMailbox mailboxImpl_;
    uint8_t mailboxQueueBuffer[ActorMailbox::ItemSize * T::QueueSize];

public:
    Actor()
        : ActorCommon<T>(mailboxImpl_)
        , mailboxImpl_(T::Name, mailboxQueueBuffer, T::QueueSize)
    {}
};

template <class T>
class PooledActor : public ActorCommon<T>
{
public:
    PooledActor(_In_ ActorMailbox& mailbox)
        : ActorCommon<T>(mailbox)
    {}
};

class ActorImpl
{
public:
    static constexpr const char* Name = "actor";
    static constexpr uint32_t StackWordCount = 2048;
    static constexpr UBaseType_t Priority = 1;
    static constexpr BaseType_t QueueSize = 10;

    SactorError ProcessIncomingMessage(_In_ BaseType_t messageId, _In_opt_ void* message, _In_opt_ void* messageReply) {
        return SactorError_NoError;
    }
};

#endif

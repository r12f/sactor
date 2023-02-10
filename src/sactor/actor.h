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
    void start() {
        SACTOR_TRACE_ACTOR_START(this);
        worker_.start();
        SACTOR_TRACE_ACTOR_STARTED(this);
    }

    constexpr const char* name() const { return T::NAME; }
    T& impl() { return impl_; }
    ActorMailbox::Tx& tx() { return mailbox_.tx(); }

    template <class MessageType>
    SactorError send_sync(_In_ const MessageType& message) {
        return mailbox_.tx().send_sync(message);
    }

    template <class MessageType, class ReplyType>
    SactorError send_recv_sync(_In_ const MessageType& message, _Out_ ReplyType& reply) {
        return mailbox_.tx().send_recv_sync(message, reply);
    }

    SactorError send_async(_In_ BaseType_t message_id) {
        return mailbox_.tx().send_async(message_id);
    }

protected:
    ActorCommon(_In_ ActorMailbox& mailbox)
        : mailbox_(mailbox)
        , worker_(mailbox, impl_)
    {
        impl_.mailbox_ = &mailbox;
        SACTOR_TRACE_ACTOR_CREATED(this);
    }
};

template <class T>
class Actor : public ActorCommon<T>
{
    ActorMailbox mailbox_impl_;
    uint8_t mailbox_queue_buffer[ActorMailbox::ItemSize * T::QUEUE_SIZE];

public:
    Actor()
        : ActorCommon<T>(mailbox_impl_)
        , mailbox_impl_(T::NAME, mailbox_queue_buffer, T::QUEUE_SIZE)
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
    template <class T>
    friend class ActorCommon;

    ActorMailbox* mailbox_;

public:
    static constexpr const char* NAME = "actor";
    static constexpr uint32_t STACK_WORD_COUNT = 2048;
    static constexpr UBaseType_t PRIORITY = 1;
    static constexpr BaseType_t QUEUE_SIZE = 10;

    SactorError ProcessIncomingMessage(_In_ BaseType_t message_id, _In_opt_ void* message, _In_opt_ void* message_reply) {
        return SactorError_NoError;
    }

protected:
    SactorError queue_delayed_message(_In_ BaseType_t message_id, _In_ uint32_t delay_in_ms);
};

#endif

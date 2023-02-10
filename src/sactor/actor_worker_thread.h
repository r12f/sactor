#ifndef LACTOR_ACTOR_DRIVER_H
#define LACTOR_ACTOR_DRIVER_H

#include "sactor/common.h"
#include "sactor/actor_mailbox.h"
#include "sactor/messaging.h"

class ActorWorkerThreadBase
{
    const char* name_;
    TaskHandle_t task_;
    StaticTask_t task_ctrl_;
    ActorMailbox& mailbox_;
    ActorMailbox::Rx::OnMessageFunc on_message_;
    void* on_message_param_;

protected:
    ActorWorkerThreadBase(_In_ const char* name, _In_ ActorMailbox& mailbox, _In_ ActorMailbox::Rx::OnMessageFunc on_message, _In_ void* on_message_param);

    // We don't need to make this function virtual, because we never cast the ActorWorkerThread pointer to ActorWorkerThreadBase pointer.
    ~ActorWorkerThreadBase();

    const char* name() const;
    ActorMailbox& get_mailbox();

    void start_with_params(_In_ uint32_t stack_word_count, _In_ StackType_t* stack_buffer, _In_ UBaseType_t priority);

private:
    static void actor_worker_task_proc(_In_ void* parameter);
    void task_proc();
};

template <class T>
class ActorWorkerThread : public ActorWorkerThreadBase
{
    StackType_t stack_buffer_[T::STACK_WORD_COUNT];
    T& actor_impl_;

public:
    ActorWorkerThread(_In_ ActorMailbox& mailbox, _In_ T& actor_impl)
        : ActorWorkerThreadBase(T::NAME, mailbox, DispatchIncomingMessageProc, (void*)this)
        , actor_impl_(actor_impl)
    {
        SACTOR_TRACE_ACTOR_WORKER_THREAD_CREATED(this);
    }

    SactorError start() { 
        start_with_params(T::STACK_WORD_COUNT, stack_buffer_, T::PRIORITY);
        return get_mailbox().tx().send_async(MESSAGE_ID_INIT);
    }

private:
    static SactorError DispatchIncomingMessageProc(_In_ void* parameter, _In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer) {
        ActorWorkerThread* worker = (ActorWorkerThread*)parameter;
        return worker->DispatchIncomingMessage(message_id, message_buffer, reply_buffer);
    }

    SactorError DispatchIncomingMessage(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer) {
        return actor_impl_.ProcessIncomingMessage(message_id, message_buffer, reply_buffer);
    }
};

#endif

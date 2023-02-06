#ifndef LACTOR_ACTOR_DRIVER_H
#define LACTOR_ACTOR_DRIVER_H

#include "sactor/common.h"
#include "freertos/task.h"
#include "sactor/actor_mailbox.h"
#include "sactor/messaging.h"

class ActorWorkerThreadBase
{
    const char* name_;
    TaskHandle_t task_;
    StaticTask_t taskCtrl_;
    ActorMailbox& mailbox_;
    ActorMailbox::Rx::OnMessageFunc onMessage_;
    void* onMessageParam_;

protected:
    ActorWorkerThreadBase(_In_ const char* name, _In_ ActorMailbox& mailbox, _In_ ActorMailbox::Rx::OnMessageFunc onMessage, _In_ void* onMessageParam);

    // We don't need to make this function virtual, because we never cast the ActorWorkerThread pointer to ActorWorkerThreadBase pointer.
    ~ActorWorkerThreadBase();

    ActorMailbox& GetMailbox();

    void StartWithParams(_In_ uint32_t stackWordCount, _In_ StackType_t* stackBuffer, _In_ UBaseType_t priority);

private:
    static void ActorWorkerTaskProc(_In_ void* parameter);
    void TaskProc();
};

template <class T>
class ActorWorkerThread : public ActorWorkerThreadBase
{
    StackType_t stackBuffer_[T::StackWordCount];
    T& actorImpl_;

public:
    ActorWorkerThread(_In_ ActorMailbox& mailbox, _In_ T& actorImpl)
        : ActorWorkerThreadBase(T::Name, mailbox, DispatchIncomingMessageProc, (void*)this)
        , actorImpl_(actorImpl)
    {
        SACTOR_TRACE_ACTOR_WORKER_THREAD_CREATED(T::Name, this);
    }

    void Start() { 
        StartWithParams(T::StackWordCount, stackBuffer_, T::Priority);
        GetMailbox().Tx().SendAsync(MESSAGE_ID_INIT);
    }

private:
    static SactorError DispatchIncomingMessageProc(_In_ void* parameter, _In_ BaseType_t messageId, _In_opt_ const void* messageBuffer, _Out_opt_ void* replyBuffer) {
        ActorWorkerThread* worker = (ActorWorkerThread*)parameter;
        return worker->DispatchIncomingMessage(messageId, messageBuffer, replyBuffer);
    }

    SactorError DispatchIncomingMessage(_In_ BaseType_t messageId, _In_opt_ const void* messageBuffer, _Out_opt_ void* replyBuffer) {
        return actorImpl_.ProcessIncomingMessage(messageId, messageBuffer, replyBuffer);
    }
};

#endif
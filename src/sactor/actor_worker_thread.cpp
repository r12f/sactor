#include "sactor/actor_worker_thread.h"

ActorWorkerThreadBase::ActorWorkerThreadBase(_In_ const char* name, _In_ ActorMailbox& mailbox, _In_ ActorMailbox::Rx::OnMessageFunc onMessage, _In_ void* onMessageParam)
    : name_(name)
    , task_(nullptr)
    , mailbox_(mailbox)
    , onMessage_(onMessage)
    , onMessageParam_(onMessageParam)
{
}

ActorWorkerThreadBase::~ActorWorkerThreadBase()
{
    if (task_ != nullptr) {
        vTaskDelete(task_);
        task_ = nullptr;
    }
}

ActorMailbox& ActorWorkerThreadBase::GetMailbox()
{
    return mailbox_;
}

void ActorWorkerThreadBase::StartWithParams(_In_ uint32_t stackWordCount, _In_ StackType_t* stackBuffer, _In_ UBaseType_t priority)
{
    SACTOR_TRACE_ACTOR_WORKER_THREAD_START(name_, this);

    task_ = xTaskCreateStatic(
        &ActorWorkerThreadBase::ActorWorkerTaskProc,
        name_,
        stackWordCount,
        (void*)this,
        priority,
        stackBuffer,
        &taskCtrl_);

    SACTOR_TRACE_ACTOR_WORKER_THREAD_STARTED(name_, this, task_);

    SACTOR_ENSURES(task_ != nullptr);
}

void ActorWorkerThreadBase::ActorWorkerTaskProc(_In_ void* parameter)
{
    ActorWorkerThreadBase* worker = (ActorWorkerThreadBase*)(parameter);
    worker->TaskProc();
}

void ActorWorkerThreadBase::TaskProc()
{
    SACTOR_TRACE_ACTOR_WORKER_THREAD_TASK_LOOP_ENTERED(name_, this);

    for (;;) {
        SactorError result = mailbox_.Rx().DispatchOneMessage(onMessage_, onMessageParam_);
        if (result == SactorError_TaskStopped) {
            break;
        }
    }
}
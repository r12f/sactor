#include "sactor/actor_worker_thread.h"

ActorWorkerThreadBase::ActorWorkerThreadBase(_In_ const char* name, _In_ ActorMailbox& mailbox, _In_ ActorMailbox::Rx::OnMessageFunc on_message, _In_ void* on_message_param)
    : name_(name)
    , task_(nullptr)
    , mailbox_(mailbox)
    , on_message_(on_message)
    , on_message_param_(on_message_param)
{
}

ActorWorkerThreadBase::~ActorWorkerThreadBase()
{
    if (task_ != nullptr) {
        vTaskDelete(task_);
        task_ = nullptr;
    }
}

const char* ActorWorkerThreadBase::get_name() const
{
    return name_;
}

ActorMailbox& ActorWorkerThreadBase::get_mailbox()
{
    return mailbox_;
}

void ActorWorkerThreadBase::start_with_params(_In_ uint32_t stack_word_count, _In_ StackType_t* stack_buffer, _In_ UBaseType_t priority)
{
    SACTOR_TRACE_ACTOR_WORKER_THREAD_START(this);

    task_ = xTaskCreateStatic(
        &ActorWorkerThreadBase::actor_worker_task_proc,
        name_,
        stack_word_count,
        (void*)this,
        priority,
        stack_buffer,
        &task_ctrl_);

    SACTOR_TRACE_ACTOR_WORKER_THREAD_STARTED(this, task_);

    SACTOR_ENSURES(task_ != nullptr);
}

void ActorWorkerThreadBase::actor_worker_task_proc(_In_ void* parameter)
{
    ActorWorkerThreadBase* worker = (ActorWorkerThreadBase*)(parameter);
    worker->task_proc();
}

void ActorWorkerThreadBase::task_proc()
{
    SACTOR_TRACE_ACTOR_WORKER_THREAD_TASK_LOOP_ENTERED(this);

    for (;;) {
        SactorError result = mailbox_.rx().dispatch_one_message(on_message_, on_message_param_);
        if (result == SactorError_TaskStopped) {
            break;
        }
    }
}

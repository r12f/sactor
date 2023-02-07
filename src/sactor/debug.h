#ifndef SACTOR_DEBUG_H
#define SACTOR_DEBUG_H

#ifndef _In_
#define _In_
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Inout_
#define _Inout_ 
#endif

#ifndef SACTOR_TRACE_LOG
    #ifdef SACTOR_ENABLE_TRACE_LOG
        #define SACTOR_TRACE_LOG(Component, Name, Pointer, Format, ...) printf("[%8s][%15s @ %p] " Format "\n", Component, Name, (void*)Pointer, ##__VA_ARGS__)
    #else
        #define SACTOR_TRACE_LOG(Component, Name, Pointer, Format, ...)
    #endif
#endif

//
// Actor
//
#define SACTOR_TRACE_ACTOR_LOG(Name, Pointer, Format, ...) \
    SACTOR_TRACE_LOG("Actor", Name, Pointer, Format, ##__VA_ARGS__)

#ifndef SACTOR_TRACE_ACTOR_CREATED
#define SACTOR_TRACE_ACTOR_CREATED(Name, Pointer) \
    SACTOR_TRACE_ACTOR_LOG(Name, Pointer, "Actor created.")
#endif

#ifndef SACTOR_TRACE_ACTOR_START
#define SACTOR_TRACE_ACTOR_START(Name, Pointer) \
    SACTOR_TRACE_ACTOR_LOG(Name, Pointer, "Starting actor.")
#endif

#ifndef SACTOR_TRACE_ACTOR_STARTED
#define SACTOR_TRACE_ACTOR_STARTED(Name, Pointer) \
    SACTOR_TRACE_ACTOR_LOG(Name, Pointer, "Actor started.")
#endif

//
// Actor worker thread
//
#define SACTOR_TRACE_ACTOR_WORKER_LOG(Name, Pointer, Format, ...) \
    SACTOR_TRACE_LOG("Worker", Name, Pointer, Format, ##__VA_ARGS__)

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_CREATED
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_CREATED(Name, Pointer) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Name, Pointer, "Actor worker thread created.")
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_START
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_START(Name, Pointer) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Name, Pointer, "Starting actor worker thread.")
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_STARTED
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_STARTED(Name, Pointer, Handle) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Name, Pointer, "Actor worker thread started: Handle = %p.", Handle)
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_TASK_LOOP_ENTERED
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_TASK_LOOP_ENTERED(Name, Pointer) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Name, Pointer, "Actor worker thread task loop entered.")
#endif

//
// Actor mailbox
//
#define SACTOR_TRACE_ACTOR_MAILBOX_LOG(Name, Pointer, Format, ...) \
    SACTOR_TRACE_LOG("Mailbox", Name, Pointer, Format, ##__VA_ARGS__)

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_CREATED
#define SACTOR_TRACE_ACTOR_MAILBOX_CREATED(Name, Pointer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Name, Pointer, "Actor mailbox created.")
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_QUEUE_MESSAGE
#define SACTOR_TRACE_ACTOR_MAILBOX_QUEUE_MESSAGE(Name, Pointer, MessageId, MessageBuffer, ReplyBuffer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Name, Pointer, "Actor mailbox queuing message: Id = %d, Buffer = %p, Reply = %p.", MessageId, MessageBuffer, ReplyBuffer)
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE
#define SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE(Name, Pointer, MessageId, MessageBuffer, ReplyBuffer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Name, Pointer, "Actor mailbox on message: Id = %d, Buffer = %p, Reply = %p.", MessageId, MessageBuffer, ReplyBuffer)
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE_COMPLETED
#define SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE_COMPLETED(Name, Pointer, MessageId, MessageBuffer, ReplyBuffer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Name, Pointer, "Actor mailbox on message completed: Id = %d, Buffer = %p, Reply = %p.", MessageId, MessageBuffer, ReplyBuffer)
#endif

//
// Delay message service
//
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, Format, ...) \
    SACTOR_TRACE_LOG("DMS", "DMS", Pointer, Format, ##__VA_ARGS__)

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATE
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATE(Pointer) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Creating delay message service.")
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATED
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATED(Pointer) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service created.")
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUE
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUE(Pointer, ActorName, ActorMailbox, MessageId, DelayInMs) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service queuing delayed message: Actor = %s, Mailbox = %p, MessageId = %d, Delay = %d ms.", ActorName, ActorMailbox, MessageId, (int)DelayInMs)
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUED
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUED(Pointer, ActorName, ActorMailbox, MessageId, DelayInMs) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service delayed message queued: Actor = %s, Mailbox = %p, MessageId = %d, Delay = %d ms.", ActorName, ActorMailbox, MessageId, (int)DelayInMs)
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOCK_TAKE_TIMEOUT
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOCK_TAKE_TIMEOUT(Pointer) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service take lock timeout.")
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULE
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULE(Pointer, NewPeriod) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service scheduling timer: NewPeriod = %d ms.", (int)NewPeriod)
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULED
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULED(Pointer, NewPeriod) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service timer scheduled: NewPeriod = %d ms.", (int)NewPeriod)
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOP
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOP(Pointer) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service stopping timer.");
#endif

#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOPPED
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOPPED(Pointer) \
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, "Delay message service timer stopped.");
#endif

//
// Design by contract assertions
//
#define SACTOR_ASSERT(AssertType, cond) do { \
    bool r = (cond); \
    if (!r) { \
        SACTOR_TRACE_LOG("Assert", AssertType, nullptr, "Assertion failed: %s", #cond); \
        configASSERT(false); \
    } \
} while (0)

#define SACTOR_REQUIRES(cond) SACTOR_ASSERT("Requires", cond)
#define SACTOR_ENSURES(cond) SACTOR_ASSERT("Ensures", cond)
#define SACTOR_UNREACHABLE() SACTOR_ASSERT("Unreachable", false)


#endif

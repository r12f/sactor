#ifndef SACTOR_DEBUG_H
#define SACTOR_DEBUG_H

//
// SAL annonations
//
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

//
// Logging
//
#ifndef SACTOR_LOG
    #define SACTOR_LOG(Level, Component, Name, Pointer, Format, ...) printf("[%s][%8s][%15s @ %p] " Format "\n", Level, Component, Name, (void*)Pointer, ##__VA_ARGS__)
#else
    #define SACTOR_LOG(Level, Component, Name, Pointer, Format, ...)
#endif

#define SACTOR_LOG_LEVEL_NONE (0)
#define SACTOR_LOG_LEVEL_FATAL (1)
#define SACTOR_LOG_LEVEL_ERROR (2)
#define SACTOR_LOG_LEVEL_WARN (3)
#define SACTOR_LOG_LEVEL_INFO (4)
#define SACTOR_LOG_LEVEL_DEBUG (5)
#define SACTOR_LOG_LEVEL_VERBOSE (6)

#ifndef SACTOR_LOG_LEVEL
#define SACTOR_LOG_LEVEL SACTOR_LOG_LEVEL_VERBOSE
#endif

#if (SACTOR_LOG_LEVEL >= SACTOR_LOG_LEVEL_FATAL)
    #ifndef SACTOR_LOGF
        #define SACTOR_LOGF(Component, Name, Pointer, Format, ...) SACTOR_LOG("F", Component, Name, Pointer, Format, ##__VA_ARGS__)
    #else
        #define SACTOR_LOGF(Component, Name, Pointer, Format, ...)
    #endif
#endif

#if (SACTOR_LOG_LEVEL >= SACTOR_LOG_LEVEL_ERROR)
    #ifndef SACTOR_LOGE
        #define SACTOR_LOGE(Component, Name, Pointer, Format, ...) SACTOR_LOG("E", Component, Name, Pointer, Format, ##__VA_ARGS__)
    #else
        #define SACTOR_LOGE(Component, Name, Pointer, Format, ...)
    #endif
#endif

#if (SACTOR_LOG_LEVEL >= SACTOR_LOG_LEVEL_WARN)
    #ifndef SACTOR_LOGW
        #define SACTOR_LOGW(Component, Name, Pointer, Format, ...) SACTOR_LOG("W", Component, Name, Pointer, Format, ##__VA_ARGS__)
    #else
        #define SACTOR_LOGW(Component, Name, Pointer, Format, ...)
    #endif
#endif

#if (SACTOR_LOG_LEVEL >= SACTOR_LOG_LEVEL_INFO)
    #ifndef SACTOR_LOGI
        #define SACTOR_LOGI(Component, Name, Pointer, Format, ...) SACTOR_LOG("I", Component, Name, Pointer, Format, ##__VA_ARGS__)
    #else
        #define SACTOR_LOGI(Component, Name, Pointer, Format, ...)
    #endif
#endif

#if (SACTOR_LOG_LEVEL >= SACTOR_LOG_LEVEL_DEBUG)
    #ifndef SACTOR_LOGD
        #define SACTOR_LOGD(Component, Name, Pointer, Format, ...) SACTOR_LOG("D", Component, Name, Pointer, Format, ##__VA_ARGS__)
    #else
        #define SACTOR_LOGD(Component, Name, Pointer, Format, ...)
    #endif
#endif

#if (SACTOR_LOG_LEVEL >= SACTOR_LOG_LEVEL_VERBOSE)
    #ifndef SACTOR_LOGV
        #define SACTOR_LOGV(Component, Name, Pointer, Format, ...) SACTOR_LOG("V", Component, Name, Pointer, Format, ##__VA_ARGS__)
    #else
        #define SACTOR_LOGV(Component, Name, Pointer, Format, ...)
    #endif
#endif

//
// Actor
//
#ifndef SACTOR_TRACE_ACTOR_LOG
#define SACTOR_TRACE_ACTOR_LOG(Pointer, Format, ...) \
    SACTOR_LOGV("Actor", Pointer->get_name(), Pointer, Format, ##__VA_ARGS__)
#endif

#ifndef SACTOR_TRACE_ACTOR_CREATED
#define SACTOR_TRACE_ACTOR_CREATED(Pointer) \
    SACTOR_TRACE_ACTOR_LOG(Pointer, "Actor created.")
#endif

#ifndef SACTOR_TRACE_ACTOR_START
#define SACTOR_TRACE_ACTOR_START(Pointer) \
    SACTOR_TRACE_ACTOR_LOG(Pointer, "Starting actor.")
#endif

#ifndef SACTOR_TRACE_ACTOR_STARTED
#define SACTOR_TRACE_ACTOR_STARTED(Pointer) \
    SACTOR_TRACE_ACTOR_LOG(Pointer, "Actor started.")
#endif

//
// Actor worker thread
//
#ifndef SACTOR_TRACE_ACTOR_WORKER_LOG
#define SACTOR_TRACE_ACTOR_WORKER_LOG(Pointer, Format, ...) \
    SACTOR_LOGV("Worker", Pointer->get_name(), Pointer, Format, ##__VA_ARGS__)
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_CREATED
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_CREATED(Pointer) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Pointer, "Actor worker thread created.")
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_START
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_START(Pointer) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Pointer, "Starting actor worker thread.")
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_STARTED
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_STARTED(Pointer, Handle) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Pointer, "Actor worker thread started: Handle = %p.", Handle)
#endif

#ifndef SACTOR_TRACE_ACTOR_WORKER_THREAD_TASK_LOOP_ENTERED
#define SACTOR_TRACE_ACTOR_WORKER_THREAD_TASK_LOOP_ENTERED(Pointer) \
    SACTOR_TRACE_ACTOR_WORKER_LOG(Pointer, "Actor worker thread task loop entered.")
#endif

//
// Actor mailbox
//
#ifndef SACTOR_TRACE_ACTOR_MAILBOX_LOG
#define SACTOR_TRACE_ACTOR_MAILBOX_LOG(Pointer, Format, ...) \
    SACTOR_LOGV("Mailbox", Pointer->actor_name_, Pointer, Format, ##__VA_ARGS__)
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_CREATED
#define SACTOR_TRACE_ACTOR_MAILBOX_CREATED(Pointer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Pointer, "Actor mailbox created.")
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_QUEUE_MESSAGE
#define SACTOR_TRACE_ACTOR_MAILBOX_QUEUE_MESSAGE(Pointer, MessageId, MessageBuffer, ReplyBuffer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Pointer, "Actor mailbox queuing message: Id = %d, Buffer = %p, Reply = %p.", MessageId, MessageBuffer, ReplyBuffer)
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE
#define SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE(Pointer, MessageId, MessageBuffer, ReplyBuffer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Pointer, "Actor mailbox on message: Id = %d, Buffer = %p, Reply = %p.", MessageId, MessageBuffer, ReplyBuffer)
#endif

#ifndef SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE_COMPLETED
#define SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE_COMPLETED(Pointer, MessageId, MessageBuffer, ReplyBuffer) \
    SACTOR_TRACE_ACTOR_MAILBOX_LOG(Pointer, "Actor mailbox on message completed: Id = %d, Buffer = %p, Reply = %p.", MessageId, MessageBuffer, ReplyBuffer)
#endif

//
// Delay message service
//
#ifndef SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG
#define SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOG(Pointer, Format, ...) \
    SACTOR_LOGV("DMS", "DMS", Pointer, Format, ##__VA_ARGS__)
#endif

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
        SACTOR_LOGF("Assert", AssertType, nullptr, "Assertion failed: %s", #cond); \
        configASSERT(false); \
    } \
} while (0)

#define SACTOR_REQUIRES(cond) SACTOR_ASSERT("Requires", cond)
#define SACTOR_ENSURES(cond) SACTOR_ASSERT("Ensures", cond)
#define SACTOR_UNREACHABLE() SACTOR_ASSERT("Unreachable", false)


#endif

#ifndef SACTOR_DELAY_MESSAGE_SERVICE_H
#define SACTOR_DELAY_MESSAGE_SERVICE_H

#include "sactor/actor_mailbox.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

class DelayMessageService
{
    struct DelayMessageJob {
        typename ActorMailbox::Tx* Target;
        BaseType_t MessageId;
        TickType_t ExpiryTick;
    };

    StaticTimer_t timerCtrl_;
    TimerHandle_t timerHandle_;
    TickType_t scheduledTimerTick_;

    DelayMessageJob jobs_[SACTOR_DELAY_MESSAGE_SERVICE_MAX_ITEM_COUNT];
    int jobCount_;
    StaticSemaphore_t jobsLockCtrl_;
    SemaphoreHandle_t jobsLockHandle_;

public:
    DelayMessageService();

private:
    static void TimerCallbackISR(_In_ TimerHandle_t timerHandle);
    void OnTimerISR();
    bool PopDelayedMessageIfExpiredFromISR(_In_ TickType_t currentTick, _Out_ DelayMessageJob& job);
    bool PopDelayedMessageIfExpiredUnderLock(_In_ TickType_t currentTick, _Out_ DelayMessageJob& job);

    // Only actors can queue delayed messages.
    friend class ActorImpl;
    SactorError QueueDelayedMessage(_In_ ActorMailbox& mailbox, _In_ BaseType_t messageId, _In_ uint32_t delayInMs);
    SactorError QueueDelayedMessageUnderLock(_In_ ActorMailbox& mailbox, _In_ BaseType_t messageId, _In_ uint32_t delayInMs);

    void AdjustJobHeapTopDown();
    void AdjustJobHeapBottomUp();
    void AdjustScheduledTimerIfNeeded(_In_ TickType_t currentTick, _In_ bool fromISR);
    void ScheduleTimer(_In_ TickType_t currentTick, _In_ bool fromISR);
    void StopTimer(_In_ bool fromISR);
};

#endif
#ifndef SACTOR_DELAY_MESSAGE_SERVICE_H
#define SACTOR_DELAY_MESSAGE_SERVICE_H

#include "sactor/actor_mailbox.h"

class DelayMessageService
{
    struct DelayMessageJob {
        typename ActorMailbox::Tx* Target;
        BaseType_t MessageId;
        TickType_t ExpiryTick;
    };

    StaticTimer_t timer_ctrl_;
    TimerHandle_t timer_handle_;
    TickType_t scheduled_timer_tick_;

    DelayMessageJob jobs_[SACTOR_DELAY_MESSAGE_SERVICE_MAX_ITEM_COUNT];
    int job_count_;
    StaticSemaphore_t jobs_lock_ctrl_;
    SemaphoreHandle_t jobs_lock_handle_;

public:
    DelayMessageService();

private:
    static void TimerCallbackISR(_In_ TimerHandle_t timer_handle);
    void OnTimerISR();
    bool PopDelayedMessageIfExpiredFromISR(_In_ TickType_t current_tick, _Out_ DelayMessageJob& job);
    bool PopDelayedMessageIfExpiredUnderLock(_In_ TickType_t current_tick, _Out_ DelayMessageJob& job);

    // Only actors can queue delayed messages.
    friend class ActorImpl;
    SactorError QueueDelayedMessage(_In_ ActorMailbox& mailbox, _In_ BaseType_t message_id, _In_ uint32_t delay_in_ms);
    SactorError QueueDelayedMessageUnderLock(_In_ ActorMailbox& mailbox, _In_ BaseType_t message_id, _In_ uint32_t delay_in_ms);

    void AdjustJobHeapTopDown();
    void AdjustJobHeapBottomUp();
    void AdjustScheduledTimerIfNeeded(_In_ TickType_t current_tick, _In_ bool from_isr);
    void ScheduleTimer(_In_ TickType_t current_tick, _In_ bool from_isr);
    void StopTimer(_In_ bool from_isr);
};

#endif

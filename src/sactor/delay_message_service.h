#ifndef SACTOR_DELAY_MESSAGE_SERVICE_H
#define SACTOR_DELAY_MESSAGE_SERVICE_H

#include "sactor/actor_mailbox.h"

class DelayMessageService
{
    struct DelayMessageJob {
        typename ActorMailbox::Tx* target;
        BaseType_t message_id;
        TickType_t expiry_tick;
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
    static void timer_callback_isr(_In_ TimerHandle_t timer_handle);
    void on_timer_isr();
    bool pop_delayed_message_if_expired_from_isr(_In_ TickType_t current_tick, _Out_ DelayMessageJob& job);
    bool pop_delayed_message_if_expired_under_lock(_In_ TickType_t current_tick, _Out_ DelayMessageJob& job);

    // Only actors can queue delayed messages.
    friend class ActorImpl;
    SactorError queue_delayed_message(_In_ ActorMailbox& mailbox, _In_ BaseType_t message_id, _In_ uint32_t delay_in_ms);
    SactorError queue_delayed_message_under_lock(_In_ ActorMailbox& mailbox, _In_ BaseType_t message_id, _In_ uint32_t delay_in_ms);

    void adjust_job_heap_top_down();
    void adjust_job_heap_bottom_up();
    void adjust_scheduled_timer_if_needed(_In_ TickType_t current_tick, _In_ bool from_isr);
    void schedule_timer(_In_ TickType_t current_tick, _In_ bool from_isr);
    void stop_timer(_In_ bool from_isr);
};

#endif

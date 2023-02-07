#include "sactor/delay_message_service.h"

DelayMessageService::DelayMessageService()
    : job_count_(0)
{
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATE(this);

    jobs_lock_handle_ = xSemaphoreCreateMutexStatic(&jobs_lock_ctrl_);
    SACTOR_ENSURES(jobs_lock_handle_ != nullptr);

    scheduled_timer_tick_ = xTaskGetTickCount();
    timer_handle_ = xTimerCreateStatic(
        "DelayMessageServiceTimer",
        1000, /* xTimerPeriod */
        0, /* ux_auto_reload */
        (void*)this, /* pvTimerId */
        DelayMessageService::TimerCallbackISR, /* px_callback_function */
        &timer_ctrl_);

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATED(this);
}

// TimerCallback is an ISR, so we need to be careful on the API that is calls.
void DelayMessageService::timer_callback_isr(_In_ TimerHandle_t timer_handle)
{
    DelayMessageService* dms = (DelayMessageService*)pvTimerGetTimerID(timer_handle);
    dms->on_timer_isr();
}

void DelayMessageService::on_timer_isr()
{
    // Since our timer is one off shot timer, we reset the next scheduled timer tick to 0 whenever the timer is fired.
    scheduled_timer_tick_ = 0;

    TickType_t current_tick = xTaskGetTickCountFromISR();

    DelayMessageJob job;
    while (pop_delayed_message_if_expired_from_isr(current_tick, job)) {
        job.Target->send_async(job.MessageId);
    }

    adjust_scheduled_timer_if_needed(current_tick, true /* from_isr */);
}

SactorError DelayMessageService::queue_delayed_message(_In_ ActorMailbox& mailbox, _In_ BaseType_t message_id, _In_ uint32_t delay_in_ms)
{
    SACTOR_REQUIRES(jobs_lock_handle_ != nullptr);

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUE(this, mailbox.GetActorName(), &mailbox, message_id, delay_in_ms);

    if (xSemaphoreTake(jobs_lock_handle_, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS)) != pdTRUE) {
        SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOCK_TAKE_TIMEOUT(this);
        return SactorError_Timeout;
    }

    SactorError result = queue_delayed_message_under_lock(mailbox, message_id, delay_in_ms);

    BaseType_t return_lock_result = xSemaphoreGive(jobs_lock_handle_);
    SACTOR_ENSURES(return_lock_result == pdTRUE);

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUED(this, mailbox.GetActorName(), &mailbox, message_id, delay_in_ms);

    return result;
}

SactorError DelayMessageService::queue_delayed_message_under_lock(_In_ ActorMailbox& mailbox, _In_ BaseType_t message_id, _In_ uint32_t delay_in_ms)
{
    if (job_count_ == SACTOR_DELAY_MESSAGE_SERVICE_MAX_ITEM_COUNT) {
        return SactorError_QueueFull;
    }

    TickType_t current_tick = xTaskGetTickCount();

    DelayMessageJob& job = jobs_[job_count_];
    job.Target = &mailbox.Tx();
    job.MessageId = message_id;
    job.ExpiryTick = current_tick + pdMS_TO_TICKS(delay_in_ms);
    ++job_count_;

    adjust_job_heap_bottom_up();
    adjust_scheduled_timer_if_needed(current_tick, false /* from_isr */);

    return SactorError_NoError;
}

bool DelayMessageService::pop_delayed_message_if_expired_from_isr(_In_ TickType_t current_tick, _Out_ DelayMessageJob& job)
{
    SACTOR_REQUIRES(jobs_lock_handle_ != nullptr);

    if (xSemaphoreTakeFromISR(jobs_lock_handle_, nullptr) != pdTRUE) {
        SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOCK_TAKE_TIMEOUT(this);
        return false;
    }

    bool found_job = pop_delayed_message_if_expired_under_lock(current_tick, job);

    BaseType_t result = xSemaphoreGiveFromISR(jobs_lock_handle_, nullptr);
    SACTOR_ENSURES(result == pdTRUE);

    return found_job;
}

bool DelayMessageService::pop_delayed_message_if_expired_under_lock(_In_ TickType_t current_tick, _Out_ DelayMessageJob& job)
{
    if (job_count_ == 0) {
        return false;
    }

    if (current_tick < jobs_[0].ExpiryTick) {
        return false;
    }

    job = jobs_[0];

    // Adjust heap to find the next closest timer.
    --job_count_;
    if (job_count_ > 0) {
        jobs_[0] = jobs_[job_count_];
        adjust_job_heap_top_down();
    }

    return true;
}

void DelayMessageService::adjust_job_heap_top_down()
{
    int job_index = 0;
    DelayMessageJob temp = jobs_[job_index];

    int child_job_index = 2 * job_index + 1;
    while (child_job_index < job_count_) {
        // Find the smaller one to move up.
        if (child_job_index + 1 < job_count_ && jobs_[child_job_index + 1].ExpiryTick < jobs_[child_job_index].ExpiryTick) {
            ++child_job_index;
        }

        if (jobs_[child_job_index].ExpiryTick > jobs_[job_index].ExpiryTick) {
            break;
        }

        jobs_[job_index] = jobs_[child_job_index];
        job_index = child_job_index;
        child_job_index = 2 * job_index + 1;
    }

    jobs_[job_index] = temp;
}

void DelayMessageService::adjust_job_heap_bottom_up()
{
    SACTOR_REQUIRES(job_count_ > 0);

    int job_index = job_count_ - 1;
    DelayMessageJob temp = jobs_[job_index];

    int parent_job_index = (job_index - 1) / 2;
    while (parent_job_index >= 0 && job_index != 0) {
        if (jobs_[parent_job_index].ExpiryTick <= jobs_[job_index].ExpiryTick) {
            break;
        }

        jobs_[job_index] = jobs_[parent_job_index];
        job_index = parent_job_index;
        parent_job_index = (job_index - 1) / 2;
    }

    jobs_[job_index] = temp;
}

void DelayMessageService::adjust_scheduled_timer_if_needed(_In_ TickType_t current_tick, _In_ bool from_isr)
{
    SACTOR_REQUIRES(timer_handle_ != nullptr);

    // If no more jobs, stop the timer.
    if (job_count_ == 0) {
        if (xTimerIsTimerActive(timer_handle_)) {
            stop_timer(from_isr);
        }

        return;
    }

    // If has job, check the closest job and set the timer to it.
    schedule_timer(current_tick, from_isr);
}

void DelayMessageService::schedule_timer(_In_ TickType_t current_tick, _In_ bool from_isr)
{
    SACTOR_REQUIRES(job_count_ > 0);

    // If current scheduled tick is the same as the required one, don't do anything.
    TickType_t expiry_tick = jobs_[0].ExpiryTick;
    if (expiry_tick == scheduled_timer_tick_) {
        return;
    }

    TickType_t new_deadline_tick = expiry_tick < current_tick ? 1 : (expiry_tick - current_tick);
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULE(this, new_deadline_tick);
    
    // Otherwise change the period and reset the timer.
    if (from_isr) {
        xTimerChangePeriodFromISR(timer_handle_, new_deadline_tick, nullptr);
        xTimerStartFromISR(timer_handle_, nullptr);
    } else {
        BaseType_t result = xTimerChangePeriod(timer_handle_, new_deadline_tick, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS));
        SACTOR_ENSURES(result == pdPASS);

        result = xTimerStart(timer_handle_, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS));
        SACTOR_ENSURES(result == pdPASS);
    }

    scheduled_timer_tick_ = expiry_tick;

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULED(this, new_deadline_tick);
}

void DelayMessageService::stop_timer(_In_ bool from_isr)
{
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOP(this);

    if (from_isr) {
        xTimerStopFromISR(timer_handle_, nullptr);
    } else {
        BaseType_t result = xTimerStop(timer_handle_, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS));
        SACTOR_ENSURES(result == pdPASS);
    }

    scheduled_timer_tick_ = 0;

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOPPED(this);
}

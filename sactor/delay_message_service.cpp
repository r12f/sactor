#include "delay_message_service.h"

DelayMessageService::DelayMessageService()
    : jobCount_(0)
{
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATE(this);

    jobsLockHandle_ = xSemaphoreCreateMutexStatic(&jobsLockCtrl_);
    SACTOR_ENSURES(jobsLockHandle_ != nullptr);

    scheduledTimerTick_ = xTaskGetTickCount();
    timerHandle_ = xTimerCreateStatic(
        "DelayMessageServiceTimer",
        1000, /* xTimerPeriod */
        0, /* uxAutoReload */
        (void*)this, /* pvTimerId */
        DelayMessageService::TimerCallbackISR, /* pxCallbackFunction */
        &timerCtrl_);

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_CREATED(this);
}

// TimerCallback is an ISR, so we need to be careful on the API that is calls.
void DelayMessageService::TimerCallbackISR(_In_ TimerHandle_t timerHandle)
{
    DelayMessageService* dms = (DelayMessageService*)pvTimerGetTimerID(timerHandle);
    dms->OnTimerISR();
}

void DelayMessageService::OnTimerISR()
{
    // Since our timer is one off shot timer, we reset the next scheduled timer tick to 0 whenever the timer is fired.
    scheduledTimerTick_ = 0;

    TickType_t currentTick = xTaskGetTickCountFromISR();

    DelayMessageJob job;
    while (PopDelayedMessageIfExpiredFromISR(currentTick, job)) {
        job.Target->SendAsync(job.MessageId);
    }

    AdjustScheduledTimerIfNeeded(currentTick, true /* fromISR */);
}

SactorError DelayMessageService::QueueDelayedMessage(_In_ ActorMailbox& mailbox, _In_ BaseType_t messageId, _In_ uint32_t delayInMs)
{
    SACTOR_REQUIRES(jobsLockHandle_ != nullptr);

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUE(this, mailbox.GetActorName(), &mailbox, messageId, delayInMs);

    if (xSemaphoreTake(jobsLockHandle_, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS)) != pdTRUE) {
        SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOCK_TAKE_TIMEOUT(this);
        return SactorError_Timeout;
    }

    SactorError result = QueueDelayedMessageUnderLock(mailbox, messageId, delayInMs);

    BaseType_t returnLockResult = xSemaphoreGive(jobsLockHandle_);
    SACTOR_ENSURES(returnLockResult == pdTRUE);

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_DELAY_MESSAGE_QUEUED(this, mailbox.GetActorName(), &mailbox, messageId, delayInMs);

    return result;
}

SactorError DelayMessageService::QueueDelayedMessageUnderLock(_In_ ActorMailbox& mailbox, _In_ BaseType_t messageId, _In_ uint32_t delayInMs)
{
    if (jobCount_ == SACTOR_DELAY_MESSAGE_SERVICE_MAX_ITEM_COUNT) {
        return SactorError_QueueFull;
    }

    TickType_t currentTick = xTaskGetTickCount();

    DelayMessageJob& job = jobs_[jobCount_];
    job.Target = &mailbox.Tx();
    job.MessageId = messageId;
    job.ExpiryTick = currentTick + pdMS_TO_TICKS(delayInMs);
    ++jobCount_;

    AdjustJobHeapBottomUp();
    AdjustScheduledTimerIfNeeded(currentTick, false /* fromISR */);

    return SactorError_NoError;
}

bool DelayMessageService::PopDelayedMessageIfExpiredFromISR(_In_ TickType_t currentTick, _Out_ DelayMessageJob& job)
{
    SACTOR_REQUIRES(jobsLockHandle_ != nullptr);

    if (xSemaphoreTakeFromISR(jobsLockHandle_, nullptr) != pdTRUE) {
        SACTOR_TRACE_DELAY_MESSAGE_SERVICE_LOCK_TAKE_TIMEOUT(this);
        return false;
    }

    bool foundJob = PopDelayedMessageIfExpiredUnderLock(currentTick, job);

    BaseType_t result = xSemaphoreGiveFromISR(jobsLockHandle_, nullptr);
    SACTOR_ENSURES(result == pdTRUE);

    return foundJob;
}

bool DelayMessageService::PopDelayedMessageIfExpiredUnderLock(_In_ TickType_t currentTick, _Out_ DelayMessageJob& job)
{
    if (jobCount_ == 0) {
        return false;
    }

    if (currentTick < jobs_[0].ExpiryTick) {
        return false;
    }

    job = jobs_[0];

    // Adjust heap to find the next closest timer.
    --jobCount_;
    if (jobCount_ > 0) {
        jobs_[0] = jobs_[jobCount_];
        AdjustJobHeapTopDown();
    }

    return true;
}

void DelayMessageService::AdjustJobHeapTopDown()
{
    int jobIndex = 0;
    DelayMessageJob temp = jobs_[jobIndex];

    int childJobIndex = 2 * jobIndex + 1;
    while (childJobIndex < jobCount_) {
        // Find the smaller one to move up.
        if (childJobIndex + 1 < jobCount_ && jobs_[childJobIndex + 1].ExpiryTick < jobs_[childJobIndex].ExpiryTick) {
            ++childJobIndex;
        }

        if (jobs_[childJobIndex].ExpiryTick > jobs_[jobIndex].ExpiryTick) {
            break;
        }

        jobs_[jobIndex] = jobs_[childJobIndex];
        jobIndex = childJobIndex;
        childJobIndex = 2 * jobIndex + 1;
    }

    jobs_[jobIndex] = temp;
}

void DelayMessageService::AdjustJobHeapBottomUp()
{
    SACTOR_REQUIRES(jobCount_ > 0);

    int jobIndex = jobCount_ - 1;
    DelayMessageJob temp = jobs_[jobIndex];

    int parentJobIndex = (jobIndex - 1) / 2;
    while (parentJobIndex >= 0 && jobIndex != 0) {
        if (jobs_[parentJobIndex].ExpiryTick <= jobs_[jobIndex].ExpiryTick) {
            break;
        }

        jobs_[jobIndex] = jobs_[parentJobIndex];
        jobIndex = parentJobIndex;
        parentJobIndex = (jobIndex - 1) / 2;
    }

    jobs_[jobIndex] = temp;
}

void DelayMessageService::AdjustScheduledTimerIfNeeded(_In_ TickType_t currentTick, _In_ bool fromISR)
{
    SACTOR_REQUIRES(timerHandle_ != nullptr);

    // If no more jobs, stop the timer.
    if (jobCount_ == 0) {
        if (xTimerIsTimerActive(timerHandle_)) {
            StopTimer(fromISR);
        }

        return;
    }

    // If has job, check the closest job and set the timer to it.
    ScheduleTimer(currentTick, fromISR);
}

void DelayMessageService::ScheduleTimer(_In_ TickType_t currentTick, _In_ bool fromISR)
{
    SACTOR_REQUIRES(jobCount_ > 0);

    // If current scheduled tick is the same as the required one, don't do anything.
    TickType_t expiryTick = jobs_[0].ExpiryTick;
    if (expiryTick == scheduledTimerTick_) {
        return;
    }

    TickType_t newDeadlineTick = expiryTick < currentTick ? 1 : (expiryTick - currentTick);
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULE(this, newDeadlineTick);
    
    // Otherwise change the period and reset the timer.
    if (fromISR) {
        xTimerChangePeriodFromISR(timerHandle_, newDeadlineTick, nullptr);
        xTimerStartFromISR(timerHandle_, nullptr);
    } else {
        BaseType_t result = xTimerChangePeriod(timerHandle_, newDeadlineTick, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS));
        SACTOR_ENSURES(result == pdPASS);

        result = xTimerStart(timerHandle_, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS));
        SACTOR_ENSURES(result == pdPASS);
    }

    scheduledTimerTick_ = expiryTick;

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_SCHEDULED(this, newDeadlineTick);
}

void DelayMessageService::StopTimer(_In_ bool fromISR)
{
    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOP(this);

    if (fromISR) {
        xTimerStopFromISR(timerHandle_, nullptr);
    } else {
        BaseType_t result = xTimerStop(timerHandle_, pdMS_TO_TICKS(SACTOR_DELAY_MESSAGE_SERVICE_LOCK_WAIT_TIME_IN_MS));
        SACTOR_ENSURES(result == pdPASS);
    }

    scheduledTimerTick_ = 0;

    SACTOR_TRACE_DELAY_MESSAGE_SERVICE_TIMER_STOPPED(this);
}
#include "sactor/static_message_queue.h"

StaticMessageQueueTx::StaticMessageQueueTx()
    : queue_(nullptr)
{
}

void StaticMessageQueueTx::set_queue_handle(_In_ QueueHandle_t queue)
{
    queue_ = queue;
}

SactorError StaticMessageQueueTx::send(_In_ void* item)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t ret = xQueueSendToBack(queue_, item, 0);

    if (ret == errQUEUE_FULL) {
        return SactorError_QueueFull;
    } else if (ret != pdTRUE) {
        return SactorError_QueueUnknownError;
    }

    return SactorError_NoError;
}

SactorError StaticMessageQueueTx::send_from_isr(_In_ void* item)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t higher_priority_task_woken = 0;
    BaseType_t ret = xQueueSendToBackFromISR(queue_, item, &higher_priority_task_woken);

    if (ret == errQUEUE_FULL) {
        return SactorError_QueueFull;
    } else if (ret != pdTRUE) {
        return SactorError_QueueUnknownError;
    }

    return SactorError_NoError;
}

StaticMessageQueueRx::StaticMessageQueueRx()
    : queue_(nullptr)
{
}

void StaticMessageQueueRx::set_queue_handle(_In_ QueueHandle_t queue)
{
    queue_ = queue;
}

SactorError StaticMessageQueueRx::receive(_Out_ void* item, _In_ TickType_t timeout_ms)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t ret = xQueuereceive(queue_, item, timeout_ms / portTICK_PERIOD_MS);
    if (ret != pdTRUE) {
        return SactorError_QueueEmpty;
    }

    return SactorError_NoError;
}

SactorError StaticMessageQueueRx::receive_from_isr(_Out_ void* item)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t higher_priority_task_woken = 0;
    BaseType_t ret = xQueuereceive_from_isr(queue_, item, &higher_priority_task_woken);
    if (ret != pdTRUE) {
        return SactorError_QueueEmpty;
    }

    return SactorError_NoError;
}

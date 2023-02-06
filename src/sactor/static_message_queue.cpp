#include "sactor/static_message_queue.h"

StaticMessageQueueTx::StaticMessageQueueTx()
    : queue_(nullptr)
{
}

void StaticMessageQueueTx::SetQueueHandle(_In_ QueueHandle_t queue)
{
    queue_ = queue;
}

SactorError StaticMessageQueueTx::Send(_In_ void* item)
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

SactorError StaticMessageQueueTx::SendFromISR(_In_ void* item)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t higherPriorityTaskWoken = 0;
    BaseType_t ret = xQueueSendToBackFromISR(queue_, item, &higherPriorityTaskWoken);

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

void StaticMessageQueueRx::SetQueueHandle(_In_ QueueHandle_t queue)
{
    queue_ = queue;
}

SactorError StaticMessageQueueRx::Receive(_Out_ void* item, _In_ TickType_t timeoutMs)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t ret = xQueueReceive(queue_, item, timeoutMs / portTICK_PERIOD_MS);
    if (ret != pdTRUE) {
        return SactorError_QueueEmpty;
    }

    return SactorError_NoError;
}

SactorError StaticMessageQueueRx::ReceiveFromISR(_Out_ void* item)
{
    SACTOR_REQUIRES(queue_ != nullptr);

    BaseType_t higherPriorityTaskWoken = 0;
    BaseType_t ret = xQueueReceiveFromISR(queue_, item, &higherPriorityTaskWoken);
    if (ret != pdTRUE) {
        return SactorError_QueueEmpty;
    }

    return SactorError_NoError;
}
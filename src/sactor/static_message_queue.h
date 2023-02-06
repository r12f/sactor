#ifndef SACTOR_STATIC_MESSAGE_QUEUE_H
#define SACTOR_STATIC_MESSAGE_QUEUE_H

#include "sactor/common.h"

template <class ItemType>
class StaticMessageQueue;

class StaticMessageQueueTx
{
    QueueHandle_t queue_;

public:
    StaticMessageQueueTx();
    SactorError Send(_In_ void* item);
    SactorError SendFromISR(_In_ void* item);

private:
    template <class ItemType>
    friend class StaticMessageQueue;
    void SetQueueHandle(_In_ QueueHandle_t queue);
};

class StaticMessageQueueRx
{
    QueueHandle_t queue_;

public:
    StaticMessageQueueRx();
    SactorError Receive(_Out_ void* item, _In_ TickType_t timeoutMs = 0);
    SactorError ReceiveFromISR(_Out_ void* item);

private:
    template <class ItemType>
    friend class StaticMessageQueue;
    void SetQueueHandle(_In_ QueueHandle_t queue);
};

template <class ItemType>
class StaticMessageQueue
{
    QueueHandle_t queue_;

    StaticQueue_t queueCtrl_;
    uint8_t* queueBuffer_;

    StaticMessageQueueTx tx_;
    StaticMessageQueueRx rx_;

public:
    StaticMessageQueue(uint8_t* queueBuffer, uint32_t itemCount) {
        queueBuffer_ = queueBuffer;
        queue_ = xQueueCreateStatic(itemCount, sizeof(ItemType), queueBuffer, &queueCtrl_);
        SACTOR_ENSURES(queue_ != nullptr);

        tx_.SetQueueHandle(queue_);
        rx_.SetQueueHandle(queue_);
    }

    ~StaticMessageQueue() {
        if (queue_ != nullptr) {
            vQueueDelete(queue_);
            queue_ = nullptr;
        }
    }

    QueueHandle_t GetHandle() const { return queue_; }

    StaticMessageQueueTx& Tx() { return tx_; }
    StaticMessageQueueRx& Rx() { return rx_; }

private:
    StaticMessageQueue(const StaticMessageQueue&) = default;
    StaticMessageQueue(StaticMessageQueue&&) = default;
};

#endif
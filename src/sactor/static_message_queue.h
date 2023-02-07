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
    SactorError send(_In_ void* item);
    SactorError send_from_isr(_In_ void* item);

private:
    template <class ItemType>
    friend class StaticMessageQueue;
    void set_queue_handle(_In_ QueueHandle_t queue);
};

class StaticMessageQueueRx
{
    QueueHandle_t queue_;

public:
    StaticMessageQueueRx();
    SactorError receive(_Out_ void* item, _In_ TickType_t timeout_ms = 0);
    SactorError receive_from_isr(_Out_ void* item);

private:
    template <class ItemType>
    friend class StaticMessageQueue;
    void set_queue_handle(_In_ QueueHandle_t queue);
};

template <class ItemType>
class StaticMessageQueue
{
    QueueHandle_t queue_;

    StaticQueue_t queue_ctrl_;
    uint8_t* queue_buffer_;

    StaticMessageQueueTx tx_;
    StaticMessageQueueRx rx_;

public:
    StaticMessageQueue(uint8_t* queue_buffer, uint32_t item_count) {
        queue_buffer_ = queue_buffer;
        queue_ = xQueueCreateStatic(item_count, sizeof(ItemType), queue_buffer, &queue_ctrl_);
        SACTOR_ENSURES(queue_ != nullptr);

        tx_.set_queue_handle(queue_);
        rx_.set_queue_handle(queue_);
    }

    ~StaticMessageQueue() {
        if (queue_ != nullptr) {
            vQueueDelete(queue_);
            queue_ = nullptr;
        }
    }

    QueueHandle_t GetHandle() const { return queue_; }

    StaticMessageQueueTx& tx() { return tx_; }
    StaticMessageQueueRx& rx() { return rx_; }

private:
    StaticMessageQueue(const StaticMessageQueue&) = default;
    StaticMessageQueue(StaticMessageQueue&&) = default;
};

#endif

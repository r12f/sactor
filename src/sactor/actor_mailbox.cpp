#include "sactor/actor_mailbox.h"

ActorMailbox::MailboxItem::MailboxItem()
    : MailboxItem(0, nullptr, nullptr, nullptr)
{
}

ActorMailbox::MailboxItem::MailboxItem(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer, _Out_ volatile bool* completed)
    : message_id(message_id)
    , message_buffer(message_buffer)
    , reply_buffer(reply_buffer)
    , handler_result(SactorError_NoError)
    , completed(completed)
    , sender_task(xTaskGetCurrentTaskHandle())
{
}

void ActorMailbox::MailboxItem::wait_completed()
{
    static TickType_t backoff_delays[] = { 1, 5, 10, 50, 100, 500, 1000, 4000 };

    if (completed == nullptr) {
        return;
    }

    uint32_t index = 0;
    while (!*completed) {
        vTaskDelay(backoff_delays[index]);
        if (index < (sizeof(backoff_delays) / sizeof(TickType_t)) - 1) {
            ++index;
        }
    }
}

void ActorMailbox::MailboxItem::mark_completed()
{
    if (completed == nullptr) {
        return;
    }

    *completed = true;
    xTaskAbortDelay(sender_task);
}

ActorMailbox::Tx::Tx(_In_ const char* actor_name, _In_ ActorMailbox& mailbox)
    : actor_name_(actor_name)
    , queue_(mailbox.queue_)
{}

SactorError ActorMailbox::Tx::send_async(_In_ BaseType_t message_id)
{
    MailboxItem mailbox_item { message_id, nullptr, nullptr, nullptr };
    return queue_request_raw(mailbox_item);
}

SactorError ActorMailbox::Tx::send_recv_sync_raw(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer)
{
    volatile bool completed = false;
    MailboxItem mailbox_item { message_id, message_buffer, reply_buffer, &completed };

    SactorError result = queue_request_raw(mailbox_item);
    if (result != SactorError_NoError) {
        return result;
    }

    mailbox_item.wait_completed();

    return mailbox_item.handler_result;
}

SactorError ActorMailbox::Tx::queue_request_raw(_In_ const MailboxItem& mailbox_item)
{
    SACTOR_TRACE_ACTOR_MAILBOX_QUEUE_MESSAGE(this, mailbox_item.message_id, mailbox_item.message_buffer, mailbox_item.reply_buffer);
    return queue_.tx().send((void *)&mailbox_item);
}

ActorMailbox::Rx::Rx(_In_ const char* actor_name, _In_ ActorMailbox& mailbox)
    : actor_name_(actor_name)
    , queue_(mailbox.queue_)
{}

SactorError ActorMailbox::Rx::dispatch_one_message(_In_ OnMessageFunc on_message, _In_ void* parameter)
{
    MailboxItem mailbox_item;
    SactorError result = queue_.rx().receive(&mailbox_item, SACTOR_ACTOR_MAILBOX_QUEUE_RECEIVE_TIMEOUT_IN_MS);
    if (result == SactorError_QueueEmpty) {
        return result;
    }

    SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE(this, mailbox_item.message_id, mailbox_item.message_buffer, mailbox_item.reply_buffer);
    result = on_message(parameter, mailbox_item.message_id, mailbox_item.message_buffer, mailbox_item.reply_buffer);
    mailbox_item.handler_result = result;

    SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE_COMPLETED(this, mailbox_item.message_id, mailbox_item.handler_result, mailbox_item.message_buffer, mailbox_item.reply_buffer);
    mailbox_item.mark_completed();

    return result;
}

ActorMailbox::ActorMailbox(_In_ const char* actor_name, _In_ uint8_t* queue_buffer, uint32_t item_count)
    : actor_name_(actor_name)
    , queue_(queue_buffer, item_count)
    , tx_(actor_name, *this)
    , rx_(actor_name, *this)
{
    SACTOR_TRACE_ACTOR_MAILBOX_CREATED(this);
}

const char* ActorMailbox::get_actor_name() const
{
    return actor_name_;
}

ActorMailbox::Tx& ActorMailbox::tx()
{
    return tx_;
}

ActorMailbox::Rx& ActorMailbox::rx()
{
    return rx_;
}

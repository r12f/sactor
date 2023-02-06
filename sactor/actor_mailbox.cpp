#include "actor_mailbox.h"

ActorMailbox::MailboxItem::MailboxItem()
    : MailboxItem(0, nullptr, nullptr, nullptr)
{
}

ActorMailbox::MailboxItem::MailboxItem(_In_ BaseType_t messageId, _In_opt_ const void* messageBuffer, _Out_opt_ void* replyBuffer, _Out_ volatile bool* completed)
    : MessageId(messageId)
    , MessageBuffer(messageBuffer)
    , ReplyBuffer(replyBuffer)
    , Completed(completed)
    , SenderTask(xTaskGetCurrentTaskHandle())
{
}

void ActorMailbox::MailboxItem::WaitCompleted()
{
    static TickType_t backoffDelays[] = { 1, 5, 10, 50, 100, 500, 1000, 4000 };

    if (Completed == nullptr) {
        return;
    }

    uint32_t index = 0;
    while (!*Completed) {
        vTaskDelay(backoffDelays[index]);
        if (index < (sizeof(backoffDelays) / sizeof(TickType_t)) - 1) {
            ++index;
        }
    }
}

void ActorMailbox::MailboxItem::MarkCompleted()
{
    if (Completed == nullptr) {
        return;
    }

    *Completed = true;
    xTaskAbortDelay(SenderTask);
}

ActorMailbox::Tx::Tx(_In_ ActorMailbox& mailbox)
    : queue_(mailbox.queue_)
{}

SactorError ActorMailbox::Tx::SendAsync(_In_ BaseType_t messageId)
{
    MailboxItem mailboxItem { messageId, nullptr, nullptr, nullptr };
    return QueueRequestRaw(mailboxItem);
}

SactorError ActorMailbox::Tx::SendRecvSyncRaw(_In_ BaseType_t messageId, _In_opt_ const void* messageBuffer, _Out_opt_ void* replyBuffer)
{
    volatile bool completed = false;
    MailboxItem mailboxItem { messageId, messageBuffer, replyBuffer, &completed };

    SactorError result = QueueRequestRaw(mailboxItem);
    if (result != SactorError_NoError) {
        return result;
    }

    mailboxItem.WaitCompleted();

    return result;
}

SactorError ActorMailbox::Tx::QueueRequestRaw(_In_ const MailboxItem& mailboxItem)
{
    SACTOR_TRACE_ACTOR_MAILBOX_QUEUE_MESSAGE(actorName_, this, mailboxItem.MessageId, mailboxItem.MessageBuffer, mailboxItem.ReplyBuffer);
    return queue_.Tx().Send((void *)&mailboxItem);
}

ActorMailbox::Rx::Rx(_In_ ActorMailbox& mailbox)
    : queue_(mailbox.queue_)
{}

SactorError ActorMailbox::Rx::DispatchOneMessage(_In_ OnMessageFunc onMessage, _In_ void* parameter)
{
    MailboxItem mailboxItem;
    SactorError result = queue_.Rx().Receive(&mailboxItem, SACTOR_ACTOR_MAILBOX_QUEUE_RECEIVE_TIMEOUT_IN_MS);
    if (result == SactorError_MailboxEmpty) {
        return result;
    }

    SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE(actorName_, this, mailboxItem.MessageId, mailboxItem.MessageBuffer, mailboxItem.ReplyBuffer);
    result = onMessage(parameter, mailboxItem.MessageId, mailboxItem.MessageBuffer, mailboxItem.ReplyBuffer);

    SACTOR_TRACE_ACTOR_MAILBOX_ON_MESSAGE_COMPLETED(actorName_, this, mailboxItem.MessageId, mailboxItem.MessageBuffer, mailboxItem.ReplyBuffer);
    mailboxItem.MarkCompleted();

    return result;
}

ActorMailbox::ActorMailbox(_In_ const char* actorName, _In_ uint8_t* queueBuffer, uint32_t itemCount)
    : actorName_(actorName)
    , queue_(queueBuffer, itemCount)
    , tx_(*this)
    , rx_(*this)
{
    SACTOR_TRACE_ACTOR_MAILBOX_CREATED(actorName, this);
}

typename ActorMailbox::Tx& ActorMailbox::Tx()
{
    return tx_;
}

typename ActorMailbox::Rx& ActorMailbox::Rx()
{
    return rx_;
}
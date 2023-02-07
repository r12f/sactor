#ifndef SACTOR_ACTOR_MAILBOX_H
#define SACTOR_ACTOR_MAILBOX_H

#include "sactor/static_message_queue.h"

class ActorMailbox
{
    struct MailboxItem
    {
        BaseType_t MessageId;
        const void* MessageBuffer;
        void* ReplyBuffer;
        volatile bool* Completed;
        TaskHandle_t SenderTask;

        MailboxItem();
        MailboxItem(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer, _Out_ volatile bool* completed);
        void WaitCompleted();
        void MarkCompleted();
    };

public:
    class Tx
    {
        const char* actor_name_;
        StaticMessageQueue<MailboxItem>& queue_;

    public:
        template <class MessageType>
        SactorError SendSync(_In_ const MessageType& message)
        {
            return SendRecvSyncRaw(MessageType::Id, &message, nullptr);
        }

        template <class MessageType, class ReplyType>
        SactorError SendRecvSync(_In_ const MessageType& message, _Out_ ReplyType& reply)
        {
            return SendRecvSyncRaw(MessageType::Id, &message, reply);
        }

        SactorError SendAsync(_In_ BaseType_t message_id);

    private:
        // ActorMailboxTx can only be created by ActorMailbox.
        friend class ActorMailbox;
        Tx(const char* actor_name, _In_ ActorMailbox& mailbox);

        SactorError SendRecvSyncRaw(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer);
        SactorError QueueRequestRaw(_In_ const MailboxItem& mailbox_item);
    };

    class Rx
    {
        const char* actor_name_;
        StaticMessageQueue<MailboxItem>& queue_;

    public:
        typedef SactorError (*OnMessageFunc)(_In_ void* parameter, _In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer);

        SactorError DispatchOneMessage(_In_ OnMessageFunc on_message, _In_ void* parameter);

    private:
        // ActorMailboxRx can only be created by ActorMailbox.
        friend class ActorMailbox;
        Rx(_In_ const char* actor_name, _In_ ActorMailbox& mailbox);
    };

private:
    const char* actor_name_;
    StaticMessageQueue<MailboxItem> queue_;
    typename ActorMailbox::Tx tx_;
    typename ActorMailbox::Rx rx_;

public:
    static const uint32_t ItemSize = sizeof(MailboxItem);

    ActorMailbox(_In_ const char* actor_name, _In_ uint8_t* queue_buffer, uint32_t item_count);
    const char* GetActorName() const;
    Tx& Tx();
    Rx& Rx();
};

#endif

#ifndef SACTOR_ACTOR_MAILBOX_H
#define SACTOR_ACTOR_MAILBOX_H

#include "sactor/static_message_queue.h"

class ActorMailbox
{
    struct MailboxItem
    {
        BaseType_t message_id;
        const void* message_buffer;
        void* reply_buffer;
        volatile bool* completed;
        TaskHandle_t sender_task;

        MailboxItem();
        MailboxItem(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer, _Out_ volatile bool* completed);

        void wait_completed();
        void mark_completed();
    };

public:
    class Tx
    {
        const char* actor_name_;
        StaticMessageQueue<MailboxItem>& queue_;

    public:
        template <class MessageType>
        SactorError send_sync(_In_ const MessageType& message)
        {
            return send_recv_sync_raw(MessageType::ID, &message, nullptr);
        }

        template <class MessageType, class ReplyType>
        SactorError send_recv_sync(_In_ const MessageType& message, _Out_ ReplyType& reply)
        {
            return send_recv_sync_raw(MessageType::ID, &message, reply);
        }

        SactorError send_async(_In_ BaseType_t message_id);

    private:
        // ActorMailboxTx can only be created by ActorMailbox.
        friend class ActorMailbox;
        Tx(const char* actor_name, _In_ ActorMailbox& mailbox);

        SactorError send_recv_sync_raw(_In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer);
        SactorError queue_request_raw(_In_ const MailboxItem& mailbox_item);
    };

    class Rx
    {
        const char* actor_name_;
        StaticMessageQueue<MailboxItem>& queue_;

    public:
        typedef SactorError (*OnMessageFunc)(_In_ void* parameter, _In_ BaseType_t message_id, _In_opt_ const void* message_buffer, _Out_opt_ void* reply_buffer);

        SactorError dispatch_one_message(_In_ OnMessageFunc on_message, _In_ void* parameter);

    private:
        // ActorMailboxRx can only be created by ActorMailbox.
        friend class ActorMailbox;
        Rx(_In_ const char* actor_name, _In_ ActorMailbox& mailbox);
    };

private:
    const char* actor_name_;
    StaticMessageQueue<MailboxItem> queue_;
    ActorMailbox::Tx tx_;
    ActorMailbox::Rx rx_;

public:
    static const uint32_t ItemSize = sizeof(MailboxItem);

    ActorMailbox(_In_ const char* actor_name, _In_ uint8_t* queue_buffer, uint32_t item_count);
    const char* get_actor_name() const;
    Tx& tx();
    Rx& rx();
};

#endif

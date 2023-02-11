set(SOURCES "src/sactor/actor.cpp"
            "src/sactor/actor_mailbox.cpp"
            "src/sactor/actor_worker_thread.cpp"
            "src/sactor/delay_message_service.cpp"
            "src/sactor/actor_mailbox.cpp"
            "src/sactor/static_message_queue.cpp")
set(INCLUDES "src")

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDES}
)

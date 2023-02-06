#ifndef SACTOR_COMMON_H
#define SACTOR_COMMON_H

#include <memory>
#include "freertos/FreeRTOS.h"
#include "sactor/config.h"
#include "sactor/debug.h"

enum SactorError {
    SactorError_NoError = 0,
    
    // Mailbox errors
    SactorError_MailboxUnknown = 1000,
    SactorError_MailboxInvalid,
    SactorError_MailboxEmpty,
    SactorError_MailboxFull,
    
    // Actor worker thread error
    SactorError_TaskStopped,
};

#endif
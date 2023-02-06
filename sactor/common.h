#ifndef SACTOR_COMMON_H
#define SACTOR_COMMON_H

#include <memory>
#include "freertos/FreeRTOS.h"
#include "sactor/config.h"
#include "sactor/debug.h"

enum SactorError {
    SactorError_NoError = 0,
    SactorError_Failed,
    SactorError_Timeout,
    
    // Mailbox errors
    SactorError_QueueUnknownError = 1000,
    SactorError_QueueInvalid,
    SactorError_QueueEmpty,
    SactorError_QueueFull,
    
    // Actor worker thread error
    SactorError_TaskStopped,
};

#endif
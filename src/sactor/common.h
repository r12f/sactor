#ifndef SACTOR_COMMON_H
#define SACTOR_COMMON_H

#ifndef SACTOR_USE_CUSTOM_FREERTOS
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/timers.h"
    #include "freertos/semphr.h"
    #include "freertos/queue.h"
#else
    #include "FreeRTOSImport.h"
#endif

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
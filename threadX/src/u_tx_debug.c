#include "u_tx_debug.h"
#include <stdio.h>

// clang-format off

/* Converts a ThreadX status macro to a printable string. */
/* This function is intended to be used with PRINTLN_...() macros, and shouldn't really ever be used outside of debugging purposes. */
/* (these macros are defined in tx_api.h) */
const char* tx_status_toString(UINT status) {
    switch(status) {
        case TX_SUCCESS: return "TX_SUCCESS";
        case TX_DELETED: return "TX_DELETED";
        case TX_POOL_ERROR: return "TX_POOL_ERROR";
        case TX_PTR_ERROR: return "TX_PTR_ERROR";
        case TX_WAIT_ERROR: return "TX_WAIT_ERROR";
        case TX_SIZE_ERROR: return "TX_SIZE_ERROR";
        case TX_GROUP_ERROR: return "TX_GROUP_ERROR";
        case TX_NO_EVENTS: return "TX_NO_EVENTS";
        case TX_OPTION_ERROR: return "TX_OPTION_ERROR";
        case TX_QUEUE_ERROR: return "TX_QUEUE_ERROR";
        case TX_QUEUE_EMPTY: return "TX_QUEUE_EMPTY";
        case TX_QUEUE_FULL: return "TX_QUEUE_FULL";
        case TX_SEMAPHORE_ERROR: return "TX_SEMAPHORE_ERROR";
        case TX_NO_INSTANCE: return "TX_NO_INSTANCE";
        case TX_THREAD_ERROR: return "TX_THREAD_ERROR";
        case TX_PRIORITY_ERROR: return "TX_PRIORITY_ERROR";
        case TX_NO_MEMORY: return "TX_NO_MEMORY"; // Same as TX_START_ERROR
        case TX_DELETE_ERROR: return "TX_DELETE_ERROR";
        case TX_RESUME_ERROR: return "TX_RESUME_ERROR";
        case TX_CALLER_ERROR: return "TX_CALLER_ERROR";
        case TX_SUSPEND_ERROR: return "TX_SUSPEND_ERROR";
        case TX_TIMER_ERROR: return "TX_TIMER_ERROR";
        case TX_TICK_ERROR: return "TX_TICK_ERROR";
        case TX_ACTIVATE_ERROR: return "TX_ACTIVATE_ERROR";
        case TX_THRESH_ERROR: return "TX_THRESH_ERROR";
        case TX_SUSPEND_LIFTED: return "TX_SUSPEND_LIFTED";
        case TX_WAIT_ABORTED: return "TX_WAIT_ABORTED";
        case TX_WAIT_ABORT_ERROR: return "TX_WAIT_ABORT_ERROR";
        case TX_MUTEX_ERROR: return "TX_MUTEX_ERROR";
        case TX_NOT_AVAILABLE: return "TX_NOT_AVAILABLE";
        case TX_NOT_OWNED: return "TX_NOT_OWNED";
        case TX_INHERIT_ERROR: return "TX_INHERIT_ERROR";
        case TX_NOT_DONE: return "TX_NOT_DONE";
        case TX_CEILING_EXCEEDED: return "TX_CEILING_EXCEEDED";
        case TX_INVALID_CEILING: return "TX_INVALID_CEILING";
        case TX_FEATURE_NOT_ENABLED: return "TX_FEATURE_NOT_ENABLED";
        default: return "UNKNOWN_STATUS";
    }
}

/* Converts a STM32 HAL status macro to a printable string. */
/* This function is intended to be used with PRINTLN_...() macros, and shouldn't really ever be used outside of debugging purposes. */
/* (these macros are defined in stm32h5xx_hal_def.h) */
const char* hal_status_toString(HAL_StatusTypeDef status) {
    switch(status) {
        case HAL_OK: return "HAL_OK";
        case HAL_ERROR: return "HAL_ERROR";
        case HAL_BUSY: return "HAL_BUSY";
        case HAL_TIMEOUT: return "HAL_TIMEOUT";
        default: return "UNKNOWN_STATUS";
    }
}

// clang-format on
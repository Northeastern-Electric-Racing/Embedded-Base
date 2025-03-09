/**
 * @file rtos_utils.h
 * @brief Utility functions for rtos.
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef RTOS_UTILS_H
#define RTOS_UTILS_H

#include <stdbool.h>
#include "cmsis_os.h"

/**
 * @brief Queue a message and set a thread noticication flag. The message is queued with a timeout of 0.
 * 
 * @param queue Queue to put message in.
 * @param msg_ptr Pointer to message to put in queue.
 * @param thread_id ID of thread that is being notified.
 * @param flags Flag to set in thread's notification array.
 * @return osStatus_t The error code of queueing the message.
 */
 osStatus_t queue_and_set_flag(osMessageQueueId_t queue, const void *msg_ptr,
                                osThreadId_t thread_id, uint32_t flags);
#endif
/**
 * @file rtoes_utils.c
 * @brief Utility functions for rtos.
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "rtos_utils.h"

osStatus_t queue_and_set_flag(osMessageQueueId_t queue, const void *msg_ptr,
			      osThreadId_t thread_id, uint32_t flags)
{
	osStatus_t status = osMessageQueuePut(queue, msg_ptr, 0U, 0U);
	osThreadFlagsSet(thread_id, flags);
	return status;
}
#ifndef __U_TX_QUEUES_H
#define __U_TX_QUEUES_H

#include "tx_api.h"
#include "u_tx_debug.h"
#include <stdint.h>

// clang-format off

/*
 *   Basically just a wrapper for ThreadX stuff. Lets you create/configure queues.
 *
 *   Author: Blake Jackson
 */

typedef struct {
	/* PUBLIC: Queue Configuration Settings */
	/* Set these when defining an instance of this struct. */
	const CHAR *name; /* Name of the queue */
	const UINT
		message_size; /* Size of each message in the queue, in bytes. */
	const UINT capacity; /* Maximum number of messages in the queue */

	/* PRIVATE: Internal implementation - DO NOT ACCESS DIRECTLY */
	/* (should only be accessed by functions in u_tx_queues.c) */
	TX_QUEUE _TX_QUEUE; /* Queue instance. */
	size_t _bytes; /* Size of each queue message, in bytes. */
	size_t _words; /* Size of each queue message, in 32-bit words. */
} queue_t;

/**
 * @brief Initializes a ThreadX queue configured in a 'queue_t' instance.
 * 
 * @param byte_pool Pointer to the ThreadX application byte pool.
 * @param queue Pointer to the queue instance to initialize.
 */
uint8_t create_queue(TX_BYTE_POOL *byte_pool, queue_t *queue);

/**
 * @brief Adds a message to the back of a ThreadX queue.
 * 
 * @param queue Pointer to queue to add to.
 * @param message Pointer to the message to add. The size of the message should match the queue's 'message_size' setting.
 * @param wait_time The amount of time (in ticks) to wait before timing out. Can either be 'TX_NO_WAIT', 'TX_WAIT_FOREVER', or a numeric value of ticks.
 */
uint8_t queue_send(queue_t *queue, void *message, uint32_t wait_time);

/**
 * @brief Removes the frontmost message in a ThreadX queue, and places it in a buffer provided in the 'message' parameter.
 * 
 * @param queue Pointer to queue to get from.
 * @param message Pointer to a buffer to store the removed message. The size of the buffer should match the queue's 'message_size' setting.
 * @param wait_time The amount of time (in ticks) to wait before timing out. Can either be 'TX_NO_WAIT', 'TX_WAIT_FOREVER', or a numeric value of ticks.
 */
uint8_t queue_receive(queue_t *queue, void *message, uint32_t wait_time);

// clang-format on

#endif
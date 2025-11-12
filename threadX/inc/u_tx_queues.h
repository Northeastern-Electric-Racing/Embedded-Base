#ifndef __U_TX_QUEUES_H
#define __U_TX_QUEUES_H

#include "tx_api.h"
#include "u_tx_debug.h"
#include <stdint.h>

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

/* API */
uint8_t create_queue(TX_BYTE_POOL *byte_pool, queue_t *queue);
uint8_t
queue_send(queue_t *queue, void *message,
	   uint32_t wait_time); // Sends a message to the specified queue.
uint8_t queue_receive(
	queue_t *queue, void *message,
	uint32_t wait_time); // Receives a message from the specified queue

#endif
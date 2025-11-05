
#include "u_tx_queues.h"

// clang-format off

uint8_t create_queue(TX_BYTE_POOL *byte_pool, queue_t *queue)
{
	uint8_t status;
	void *pointer;

	/* Calculate message size in 32-bit words (round up), and then validate it. */
	/* According to the Azure RTOS ThreadX Docs, "message sizes range from 1 32-bit word to 16 32-bit words". */
	/* Basically, queue messages have to be a multiple of 4 bytes? Kinda weird but this should handle it. */
	UINT message_size_words = (queue->message_size + 3) / 4;
	if (message_size_words < 1 || message_size_words > 16) {
		PRINTLN_ERROR("Invalid message size %d bytes (must be 1-64 bytes). Queue: %s", queue->message_size, queue->name);
		return U_ERROR;
	}

	/* Store metadata */
	queue->_bytes = queue->message_size;
	queue->_words = message_size_words;

	/* Calculate total queue size in bytes. */
	int queue_size_bytes = message_size_words * 4 * queue->capacity;

	/* Allocate the stack for the queue. */
	status = tx_byte_allocate(byte_pool, (VOID **)&pointer, queue_size_bytes, TX_NO_WAIT);
	if (status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to allocate memory before creating queue (Status: %d/%s, Queue: %s).", status, tx_status_toString(status), queue->name);
		return U_ERROR;
	}

	/* Create the queue */
	status = tx_queue_create(&queue->_TX_QUEUE, (CHAR *)queue->name, message_size_words, pointer, queue_size_bytes);
	if (status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to create queue (Status: %d/%s, Queue: %s).", status, tx_status_toString(status), queue->name);
		tx_byte_release(pointer); // Free allocated memory if queue creation fails
		return U_ERROR;
	}

	return U_SUCCESS;
}

uint8_t queue_send(queue_t *queue, void *message)
{
	UINT status;

	/* Create a buffer. */
	uint32_t buffer[queue->_words]; // Max size is 16 words (64 bytes).
	memset(buffer, 0, sizeof(buffer)); // Initialize buffer to zero

	/* Copy message into the buffer. The buffer is what actually gets sent to the queue. */
	memcpy(buffer, message, queue->_bytes);

	/* Send message (buffer) to the queue. */
	status = tx_queue_send(&queue->_TX_QUEUE, buffer, QUEUE_WAIT_TIME);
	if (status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to send message to queue (Status: %d/%s, Queue: %s).", status, tx_status_toString(status), queue->_TX_QUEUE.tx_queue_name);
		return U_ERROR;
	}

	return U_SUCCESS;
}

uint8_t queue_receive(queue_t *queue, void *message)
{
	UINT status;

	/* Create a buffer */
	uint32_t buffer[queue->_words]; // Max size is 16 words (64 bytes).
	memset(buffer, 0, sizeof(buffer)); // Initialize buffer to zero

	/* Receive message from the queue. */
	status = tx_queue_receive(&queue->_TX_QUEUE, buffer, QUEUE_WAIT_TIME);
	if (status == TX_QUEUE_EMPTY) {
		return U_QUEUE_EMPTY;
	}

	if ((status != TX_SUCCESS)) {
		PRINTLN_ERROR("Failed to receive message from queue (Status: %d/%s, Queue: %s).", status, tx_status_toString(status), queue->_TX_QUEUE.tx_queue_name);
		return U_ERROR;
	}

	/* Copy the data from the buffer into the message pointer. Using memcpy() here prevents memory overflow. */
	memcpy(message, buffer, queue->_bytes);

	return U_SUCCESS;
}

// clang-format on
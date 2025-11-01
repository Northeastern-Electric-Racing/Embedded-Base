
#include "u_tx_threads.h"
#include "u_tx_debug.h"

uint8_t create_thread(TX_BYTE_POOL *byte_pool, thread_t *thread)
{
	CHAR *pointer;
	uint8_t status;

	/* Allocate the stack for the thread. */
	status = tx_byte_allocate(byte_pool, (VOID **)&pointer, thread->size,
				  TX_NO_WAIT);
	if (status != TX_SUCCESS) {
		DEBUG_PRINTLN(
			"ERROR: Failed to allocate stack before creating thread (Status: %d/%s, Thread: %s).",
			status, tx_status_toString(status), thread->name);
		return U_ERROR;
	}

	/* Create the thread. */
	status = tx_thread_create(&thread->_TX_THREAD, (CHAR *)thread->name,
				  thread->function, thread->thread_input,
				  pointer, thread->size, thread->priority,
				  thread->threshold, thread->time_slice,
				  thread->auto_start);
	if (status != TX_SUCCESS) {
		DEBUG_PRINTLN(
			"ERROR: Failed to create thread (Status: %d/%s, Thread: %s).",
			status, tx_status_toString(status), thread->name);
		tx_byte_release(
			pointer); // Free allocated memory if thread creation fails
		return U_ERROR;
	}

	return U_SUCCESS;
}
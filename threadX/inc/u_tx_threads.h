#ifndef __U_TX_THREADS_H
#define __U_TX_THREADS_H

#include "tx_api.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
	/* PUBLIC: Thread Configuration Settings */
	/* Set these when defining an instance of this struct. */
	const CHAR *name; /* Name of Thread */
	const ULONG
		thread_input; /* Thread Input. You can put whatever you want in here. Defaults to zero. */
	const ULONG size; /* Stack Size (in bytes) */
	const UINT priority; /* Priority */
	const UINT threshold; /* Preemption Threshold */
	const ULONG time_slice; /* Time Slice */
	const UINT auto_start; /* Auto Start */
	const UINT sleep; /* Sleep (in ticks) */
	VOID (*function)(ULONG); /* Thread Function */

	/* PRIVATE: Internal implementation - DO NOT ACCESS DIRECTLY */
	/* (should only be accessed by functions in u_tx_threads.c) */
	TX_THREAD _TX_THREAD;
} thread_t;

/* Creates a thread based on the provided configuration.
 *
 * @param byte_pool Pointer to the byte pool to allocate the thread's stack from.
 * @param thread Pointer to a thread_t struct containing the thread's configuration.
 * @return U_SUCCESS if the thread was created successfully, U_ERROR otherwise.
 */
uint8_t create_thread(TX_BYTE_POOL *byte_pool, thread_t *thread);

/**
 * Sleeps the current thread for the specified number of milliseconds.
 */
uint8_t thread_sleep_ms(uint32_t ms);

#endif
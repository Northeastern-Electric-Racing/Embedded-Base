#include "tx_api.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
    /* PUBLIC: Thread Configuration Settings */
    /* Set these when defining an instance of this struct. */
    const CHAR      *name;                /* Name of Thread */
    const ULONG     thread_input;         /* Thread Input. You can put whatever you want in here. Defaults to zero. */
    const ULONG     size;                 /* Stack Size (in bytes) */
    const UINT      priority;             /* Priority */
    const UINT      threshold;            /* Preemption Threshold */
    const ULONG     time_slice;           /* Time Slice */
    const UINT      auto_start;           /* Auto Start */
    const UINT      sleep;                /* Sleep (in ticks) */
    VOID            (*function)(ULONG);   /* Thread Function */

    /* PRIVATE: Internal implementation - DO NOT ACCESS DIRECTLY */
    /* (should only be accessed by functions in u_threads.c) */
    TX_THREAD _TX_THREAD;
} thread_t;

uint8_t create_thread(TX_BYTE_POOL *byte_pool, thread_t *thread);

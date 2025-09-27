#ifndef __U_TX_MUTEX_H
#define __U_TX_MUTEX_H

#include "tx_api.h"
#include "u_tx_general.h"
#include "u_tx_debug.h"
#include <stdint.h>
#include <stdbool.h>

#define MUTEX_WAIT_TIME TX_WAIT_FOREVER

typedef struct {
    /* PUBLIC: Mutex Configuration Settings */
    /* Set these when defining an instance of this struct. */
    const CHAR *name;            /* Name of Mutex */
    const UINT priority_inherit; /* Specifies if the mutex supports priority inheritance. See page 55 of the "Azure RTOS ThreadX User Guide". */

    /* PRIVATE: Internal implementation - DO NOT ACCESS DIRECTLY */
    /* (should only be accessed by functions in u_tx_queueses.c) */
    TX_MUTEX _TX_MUTEX;
} mutex_t;

/* API */
uint8_t mutexes_init(); // Initializes all mutexes set up in u_tx_queueses.c
uint8_t mutex_get(mutex_t *mutex); // Gets a mutex.
uint8_t mutex_put(mutex_t *mutex); // Puts a mutex.

#endif /* u_tx_queues.h */
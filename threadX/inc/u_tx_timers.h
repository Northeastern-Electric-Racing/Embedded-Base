#pragma once

// clang-format off

#include "tx_api.h"
#include <stdint.h>

typedef enum {
    ONESHOT,    /* Timer runs once. */
    PERIODIC    /* Timer runs periodically based on the 'duration' setting. */
} timer_type_t;

typedef struct {
    /* PUBLIC: Thread Configuration Settings */
	/* Set these when defining an instance of this struct. */
    const CHAR *name;        /* Name of the timer. */ 
    VOID (*callback)(ULONG); /* Callback to be ran when the timer expires. */
    ULONG duration;          /* The time until the timer expires (in ticks). */
    ULONG callback_input;    /* Optional setting. Input for the callback function. Just set to '0' if you don't need to use. */
    timer_type_t type;       /* Type of timer. See 'timer_type_t'. */
    bool auto_activate;      /* Whether or not the timer should auto-activate. */

    /* PRIVATE: Internal implementation - DO NOT ACCESS DIRECTLY */
	/* (should only be accessed by functions in u_tx_timers.c) */
    TX_TIMER TX_TIMER_;      /* The actual timer instance. */
} timer_t;

/* API */
int timer_init(timer_t* timer);
int timer_start(timer_t* timer);
int timer_stop(timer_t* timer);
int timer_reset(timer_t* timer);
int timer_restart(timer_t* timer);
int timer_getRemainingTicks(timer_t* timer, uint32_t *remaining);

// clang-format on
#pragma once

// clang-format off

#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>

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

/**
 * @brief Initializes a ThreadX timer.
 * 
 * @param timer Pointer to the timer to initialize.
 */
int timer_init(timer_t* timer);

/**
 * @brief Starts a ThreadX timer.
 * 
 * @param timer Pointer to the timer to start.
 */
int timer_start(timer_t* timer);

/**
 * @brief Stops a ThreadX timer. If the timer is already deactivated, this function will have no effect.
 * 
 * @param timer Pointer to the timer to stop.
 */
int timer_stop(timer_t* timer);

/**
 * @brief Resets a ThreadX timer back to its starting position. Does not automatically start the timer after resetting (use timer_restart() for that).
 * 
 * @param timer Pointer to the timer to reset.
 */
int timer_reset(timer_t* timer);

/**
 * @brief Restarts a timer.
 * 
 * @param timer Pointer to the timer to restart.
 */
int timer_restart(timer_t* timer);

/**
 * @brief Gets the remaining number of ticks until the timer's expiration.
 * 
 * @param timer Pointer to the timer.
 * @param remaining Buffer to store the information.
 */
int timer_getRemainingTicks(timer_t* timer, uint32_t *remaining);

// clang-format on
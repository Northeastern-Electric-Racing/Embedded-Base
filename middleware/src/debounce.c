/**
 * @file debounce.c
 * @brief Utility to debounce a signal.
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "debounce.h"

void debounce(bool input, nertimer_t *timer, uint32_t period,
	      void (*cb)(void *arg), void *arg)
{
    if (input && !is_timer_active(timer)) {
        start_timer(timer, period);
    } else if (!input && is_timer_active(timer)) {
        /* Input is no longer high, so stop timer */
        cancel_timer(timer);
    } else if (input && is_timer_expired(timer)) {
        cb(arg);
    }
}
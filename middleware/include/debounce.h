/**
 * @file debounce.h
 * @brief Utility to debounce a signal.
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdbool.h>
#include "timer.h"

/**
 * @brief Function to debounce a signal. Debounce is started and maintained by a high signal, and it is interrupted by a low signal. The callback is called in a thread context.
 * 
 * @param input Input to debounce.
 * @param timer Timer for debouncing input.
 * @param period The period of time, in milliseconds, to debounce the input for.
 * @param cb Callback to be called if the input is successfully debounced.
 * @param arg Argument of the callback function.
 */
void debounce(bool input, nertimer_t *timer, uint32_t period,
          void (*cb)(void *arg), void *arg);
#endif

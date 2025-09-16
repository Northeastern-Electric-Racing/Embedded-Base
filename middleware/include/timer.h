#ifndef TIMER_H
#define TIMER_H

#include "stm32xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t start_time;
	uint32_t end_time;
	bool active;
	bool completed;
} nertimer_t;

/**
 * @brief Starts a timer of the given lenth
 *
 * @param timer The timer to act on
 */
void start_timer(nertimer_t *timer, uint32_t duration);

/**
 * @brief cancels an active timer
 *
 * @param timer The timer to act on
 */
void cancel_timer(nertimer_t *timer);

/**
 * @brief Checks if the timer has expired
 *
 * @param timer The timer to act on
 * @return true if the timer has expired, false otherwise
 */
bool is_timer_expired(nertimer_t *timer);

/**
 * @brief Checks if the timer is active
 *
 * @param timer The timer to act on
 * @return true if the timer is active, false otherwise
 */
bool is_timer_active(nertimer_t *timer);

#endif // TIMER_H

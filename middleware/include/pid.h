#ifndef PID_H
#define PID_H

#include "stm32xx_hal.h"
#include <stdbool.h>

typedef struct {
	float kP, kI, kD;

	float err_tol;
	float err;
	float prev_err;
	float prev_time; // in seconds
	float min_out;
	float max_out;

	float integral_sum;
} pid_t;

/**
 * @brief Initializes a PID controller
 *
 * @param kP Proportional constant
 * @param kI Integral constant
 * @param kD Derivative constant
 * @param err_tol Error tolerance for checking if reached setpoint
 * @param min_out Minimum output value
 * @param max_out Maximum output value
 * @return pid_t
 */
pid_t *pid_init(float kP, float kI, float kD, float err_tol, float min_out,
		float max_out);

/**
 * @brief Returns PID output given the setpoint and current state value
 *
 * @param pid
 * @param setpoint Desired state value
 * @param measurement Current state value
 */
float pid_calculate(pid_t *pid, float setpoint, float measurement);

/**
 * @brief Returns whether or not the controller is within error tolerance
 *
 * @param pid
 * @return true
 * @return false
 */
bool pid_at_setpoint(pid_t *pid);

#endif // PID_H
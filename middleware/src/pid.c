#include "pid.h"

pid_t *pid_init(float kP, float kI, float kD, float err_tol)
{
    pid_t *pid = malloc(sizeof(pid_t));

    pid->kP = kP;
    pid->kI = kI;
    pid->kD = kD;
    pid->err_tol = err_tol;
    pid->err = 0;
    pid->prev_err = 0;
    pid->prev_time = (float)HAL_GetTick() / 1000; // convert ms to s
    pid->integral_sum = 0;

    return pid;
}

float pid_calculate(pid_t *pid, float setpoint, float measurement)
{
    float output = 0;
    float period = 0;

    // set error
    pid->err = setpoint - measurement;

    if (pid->kI > 0 || pid->kD > 0)
    {
        float curr_time = (float)HAL_GetTick() / 1000;
        period = curr_time - pid->prev_time;
        pid->prev_time = curr_time;
    }

    // proportional
    if (pid->kP > 0)
    {
        output += pid->kP * pid->err;
    }

    // + integral
    if (pid->kI > 0)
    {
        // reimann sum
        pid->integral_sum = pid->integral_sum + pid->err * period;
        output += pid->kI * pid->integral_sum;
    }

    // + derivative
    if (pid->kD > 0)
    {
        // change in error / change in time
        float derivative = (pid->err - pid->prev_err) / period;
        output += pid->kD * derivative;
        pid->prev_err = pid->err;
    }

    return output;
}

bool pid_at_setpoint(pid_t *pid)
{
    // not using abs function since it converts to int
    return (pid->err < 0 ? pid->err * -1.0 : pid->err) < pid->err_tol;
}

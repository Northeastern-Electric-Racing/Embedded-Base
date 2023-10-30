#include "timer.h"

void start_timer(timer_t *timer)
{
    /* this function assumes tick set to default 1 ms. Update or use HAL_GetTickFreq() if not the case */
    timer->start_time = HAL_GetTick();
    timer->end_time = timer->start_time + timer->duration;
    timer->active = true;
}

void cancel_timer(timer_t *timer)
{
    timer->active = false;
}

bool is_timer_expired(timer_t *timer)
{
    if (timer->active)
    {
        if (HAL_GetTick() >= timer->end_time)
        {
            timer->active = false;
            return true;
        }
    }
    return false;
}

bool is_timer_active(timer_t *timer);
{
    return timer->active;
}
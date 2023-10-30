#include "timer.h"

void start_timer(timer_t *timer, uint32_t duration)
{
    /* this function assumes tick set to default 1 ms. Update or use HAL_GetTickFreq() if not the case */
    timer->start_time = HAL_GetTick();
    timer->end_time = timer->start_time + duration;
    timer->active = true;
    timer->completed = false;
}

void cancel_timer(timer_t *timer)
{
    timer->active = false;
    timer->completed = false;
}

bool is_timer_expired(timer_t *timer)
{
    if (timer->active)
    {
        if (HAL_GetTick() >= timer->end_time)
        {
            timer->active = false;
            timer->completed = true;
        }
    }
    return timer->completed;
}

bool is_timer_active(timer_t *timer);
{
    return timer->active;
}
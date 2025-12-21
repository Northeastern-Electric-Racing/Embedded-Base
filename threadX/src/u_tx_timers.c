#include "u_tx_timers.h"
#include "u_tx_debug.h"

// clang-format off

/* Initializes a timer. */
int timer_init(timer_t* timer) {
    /* Set reschedule ticks setting. */
    ULONG reschedule_ticks;
    if(timer->type == ONESHOT) {
        reschedule_ticks = 0; // If it's a one-shot timer, don't reschedule the timer.
    } else {
        reschedule_ticks = timer->duration; // If it's a periodic timer, set the timer to go off every 'duration' ticks.
    }

    /* Set auto-activate setting. */
    UINT auto_activate;
    if(timer->auto_activate) {
        auto_activate = TX_AUTO_ACTIVATE; // The timer will start automatically at initialization.
    } else {
        auto_activate = TX_NO_ACTIVATE; // The timer must be manually started after initialization.
    }

    /* Create the timer. */
    int status = tx_timer_create(&timer->TX_TIMER_, (CHAR*)timer->name, timer->callback, timer->callback_input, timer->duration, reschedule_ticks, auto_activate);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to create timer (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }

    /* Return successful. */
    PRINTLN_INFO("Ran Timer::init() (Timer: %s).", timer->name);
    return U_SUCCESS;
}

/* Starts a timer. */
int timer_start(timer_t* timer) {
    /* Get active status. */
    bool is_active = false;
    int status = timer_isActive(timer, &is_active);
    if(status != U_SUCCESS) {
        PRINTLN_ERROR("Failed to get the activation status of a timer (Timer: %s).", timer->name);
        return U_ERROR;
    }

    /* Check active status. */
    if(is_active) {
        PRINTLN_ERROR("Tried to start a timer that is already active (Timer: %s).", timer->name);
        return U_ERROR;
    }

    /* Activate the timer. */
    status = tx_timer_activate(&timer->TX_TIMER_);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to call tx_timer_activate() (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }

    return U_SUCCESS;
}

/* Stops a timer. If the timer is already deactivated, this function will have no effect. */
int timer_stop(timer_t* timer) {
    /* Deactivate the timer. */
    int status = tx_timer_deactivate(&timer->TX_TIMER_);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to call tx_timer_deactivate() (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }
    return U_SUCCESS;
}

/* Resets a timer back to its starting position. Does not automatically start the timer after resetting (use Timer::restart() for that.) */
int timer_reset(timer_t* timer) {
    /* Deactivate the timer. */
    int status = tx_timer_deactivate(&timer->TX_TIMER_);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to call tx_timer_deactivate() (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }

    /* Set reschedule ticks setting. */
    ULONG reschedule_ticks;
    if(timer->type == ONESHOT) {
        reschedule_ticks = 0; // If it's a one-shot timer, don't reschedule the timer.
    }
    else {
        reschedule_ticks = timer->duration; // If it's a periodic timer, set the timer to go off every 'duration' ticks.
    }

    /* Change the timer. */
    status = tx_timer_change(&timer->TX_TIMER_, timer->duration, reschedule_ticks);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to call tx_timer_change() (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }

    return U_SUCCESS;
}

/* Restarts a timer. */
int timer_restart(timer_t* timer) {
    /* Reset the timer. */
    int status = timer_reset(timer);
    if(status != U_SUCCESS) {
        PRINTLN_ERROR("Failed to call timer_reset() when restarting timer (Timer: %s).", timer->name);
        return U_ERROR;
    }

    /* Start the timer. */
    status = timer_start(timer);
    if(status != U_SUCCESS) {
        PRINTLN_ERROR("Failed to call timer_start() when restarting timer (Timer: %s).", timer->name);
        return U_ERROR;
    }

    return U_SUCCESS;
}

/* Gets the remaining number of ticks until the timer's expiration. */
int timer_getRemainingTicks(timer_t* timer, uint32_t* remaining) {
    /* Get the remaining ticks status. */
    ULONG remaining_ticks;
    int status = tx_timer_info_get(&timer->TX_TIMER_, (CHAR**)TX_NULL, (UINT*)TX_NULL, &remaining_ticks, (ULONG*)TX_NULL, (TX_TIMER**)TX_NULL);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to call tx_timer_info_get (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }

    /* Set the remaining ticks. */
    *remaining = (uint32_t)(remaining_ticks);

    return U_SUCCESS;
}

/* Check if a timer is active. */
int timer_isActive(timer_t* timer, bool* active) {
    /* Get the active indication. */
    UINT is_active;
    int status = tx_timer_info_get(&timer->TX_TIMER_, (CHAR**)TX_NULL, &is_active, (ULONG*)TX_NULL, (ULONG*)TX_NULL, (TX_TIMER**)TX_NULL);
    if(status != TX_SUCCESS) {
        PRINTLN_ERROR("Failed to call tx_timer_info_get (Status: %d/%s, Timer: %s).", status, tx_status_toString(status), timer->name);
        return U_ERROR;
    }

    /* Check the indication. */
    if(is_active == TX_TRUE) {
        *active = true;
    } else {
        *active = false;
    }

    return U_SUCCESS;
}

// clang-format on
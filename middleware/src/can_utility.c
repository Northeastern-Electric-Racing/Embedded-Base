
#include "can_utility.h"

/**
 * Sends a rate limited can message
 */
HAL_StatusTypeDef send_rl_can_msg(can_t *can, rl_can_msg_t *rl_can_msg)
{
	if (osTimerIsRunning(rl_can_msg->msg_timer)) {
		return HAL_BUSY;
	}

	can_send_msg(can, &rl_can_msg->can_msg);
	osTimerStart(rl_can_msg->msg_timer,
		     pdMS_TO_TICKS(rl_can_msg->msg_rate * 1000));

	return HAL_OK;
}
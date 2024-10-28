#include "can_handler.h"
#include "FreeRTOS.h"

typedef struct {
	can_msg_t can_msg;
	osThreadId_t msg_timer;
	uint8_t msg_rate; /* in messages per second */
} rl_can_msg_t;

HAL_StatusTypeDef send_rl_can_msg(can_t *can, rl_can_msg_t *rl_can_msg);
HAL_StatusTypeDef rl_can_msg_init(rl_can_msg_t *rl_can_msg);
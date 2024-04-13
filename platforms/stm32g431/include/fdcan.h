#ifndef FDCAN_H
#define FDCAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

/* function pointer type for the callback */
typedef void (*can_callback_t)(FDCAN_HandleTypeDef *hcan);

typedef struct{
	FDCAN_HandleTypeDef *hcan;
	const uint16_t *id_list;
	uint8_t id_list_len;

	/* desired behavior varies by app - so implement this at app level */
	can_callback_t callback;
} can_t;

typedef struct{
	uint32_t id;
	uint8_t data[8];
	uint8_t len;
} can_msg_t;

HAL_StatusTypeDef can_init(can_t *can);
HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);

#endif // FDCAN_H
#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "c_utils.h"

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then implement in
 * `stm32xxxx_it.c`, which STM32CubeMX generates
 */

typedef struct{
	CAN_HandleTypeDef *hcan;
	const uint16_t *id_list;
	uint8_t id_list_len;
} can_t;

typedef struct{
	uint16_t id;
	uint8_t data[8];
	uint8_t len;
} can_msg_t;

HAL_StatusTypeDef can_init(can_t *can);
HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);

#endif // CAN_H
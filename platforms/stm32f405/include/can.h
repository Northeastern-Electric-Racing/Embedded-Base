#ifndef CAN_H
#define CAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "c_utils.h"
#include "stm32xx_hal.h"

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

typedef struct {
	CAN_HandleTypeDef *hcan;
} can_t;

typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint8_t len;
} can_msg_t;

HAL_StatusTypeDef can_init(can_t *can);
HAL_StatusTypeDef can_add_filter(can_t *can, uint32_t *id_list);
HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);
HAL_StatusTypeDef can_send_extended_msg(can_t *can, can_msg_t *msg);

#endif // CAN_H
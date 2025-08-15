#ifndef FDCAN_H
#define FDCAN_H

// clang-format off
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32xx_hal.h"

typedef struct
{
	uint32_t id;
	uint8_t data[8]; // technically can go to 64 bytes but i cant count that high
	bool id_is_extended;
	uint8_t len;
} can_msg_t;

typedef struct
{
	FDCAN_HandleTypeDef *hcan;

	uint32_t standard_filter_index;
	uint32_t extended_filter_index;
} can_t;

HAL_StatusTypeDef can_init(can_t *can);
HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);
HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint16_t can_ids[2]);
HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t can_ids[2]);

// clang-format on
#endif // FDCAN_H
#ifndef CAN_H
#define CAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "c_utils.h"
#include "stm32xx_hal.h"
#include <stdbool.h>

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

typedef struct {
	CAN_HandleTypeDef *hcan;
} can_t;

typedef struct {
	union {
		uint16_t standard_id;
		uint32_t extended_id;
	} id;
	bool is_extended;
	uint8_t data[8];
	uint8_t len;
} can_msg_t;

HAL_StatusTypeDef can_init(can_t *can);

/**
 * @brief Add CANIDs to the CAN whitelist filter. Up to 7 additions of 4
 * IDs can be done. Only supports CAN standard IDs.
 * 
 * @param can CAN datastruct.
 * @param id_list List of CAN IDs to whitelist. Must be an array of four
 * elements.
 * @return HAL_StatusTypeDef Error code.
 */
HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint32_t id_list[4]);

/**
 * @brief Add CANIDs to the CAN whitelist filter. Up to 8 additions of 2
 * IDs can be done. Only supports CAN extended IDs.
 * 
 * @param can CAN datastruct.
 * @param id_list List of CAN IDs to whitelist. Must be an array of two
 * elements.
 * @return HAL_StatusTypeDef Error code.
 */
HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t id_list[2]);

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);

#endif // CAN_H
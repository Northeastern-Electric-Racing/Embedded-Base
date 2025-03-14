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
	uint32_t filter_bank;
} can_t;

/**
 * @brief Struct containing all relevant information for a CAN message.
 * 
 * @param id The message's CAN ID. (uint32_t)
 * @param id_is_extended Denotes whether a standard or extended CAN ID is being used. Defaults to false, so ignore if not using an extended ID. (bool)
 * @param data The message's data. The maximum is 8 bytes. (uint8_t array)
 * @param len The length of the data in bytes. (uint8_t)
 */
typedef struct {
	uint32_t id;
	bool id_is_extended;
	uint8_t data[8];
	uint8_t len;
} can_msg_t;

HAL_StatusTypeDef can_init(can_t *can);

/**
 * @brief Adds a standard CAN ID filter to the CAN bus. A standard CAN ID filter can contain up to 4 IDs. IDs cannot be larger than 11 bits.
 * 
 * @param can The CAN bus to add the filter to. (can_t)
 * @param can_id_list An array of 4 standard CAN IDs to filter. (uint16_t array)
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR if not. (HAL_StatusTypeDef)
 */
HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint16_t can_id_list[4]);

/**
 * @brief Adds an extended CAN ID filter to the CAN bus. An extended CAN ID filter can contain up to 2 IDs. IDs cannot be larger than 29 bits.
 * 
 * @param can The CAN bus to add the filter to. (can_t)
 * @param can_id_list An array of 2 extended CAN IDs to filter. (uint32_t array)
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR if not. (HAL_StatusTypeDef)
 */
HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t can_id_list[2]);

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);

#endif // CAN_H
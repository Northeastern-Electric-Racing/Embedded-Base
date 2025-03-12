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
 * @brief Adds a single CAN ID to the filter bank.
 * 
 * @param hcan The CAN handle. (CAN_HandleTypeDef)
 * @param can_id The CAN ID to add to the filter bank. (uint32_t)
 * @param is_extended Denotes whether a standard or extended CAN ID is being used. (bool)
 * @return Error status
 * 
 */
static HAL_StatusTypeDef can_add_filter(can_t *can, uint32_t can_id,
					bool is_extended);

/**
 * @brief Adds a list of standard CAN IDs to the filter bank.
 * 
 * @param hcan The CAN handle. (CAN_HandleTypeDef)
 * @param can_id_list The list of CAN IDs. (uint32_t array)
 * @param can_id_list_len The length of the list. (uint8_t)
 * @return Error status
 */
HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint32_t *can_id_list,
					  uint8_t can_id_list_len);

/**
 * @brief Adds a list of extended CAN IDs to the filter bank.
 * 
 * @param hcan The CAN handle. (CAN_HandleTypeDef)
 * @param can_id_list The list of CAN IDs. (uint32_t array)
 * @param can_id_list_len The length of the list. (uint8_t)
 * @return Error status
 */
HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t *can_id_list,
					  uint8_t can_id_list_len);

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);

#endif // CAN_H
#ifndef _U_TX_CAN_H
#define _U_TX_CAN_H

#include "stm32xx_hal.h"
#include <stdint.h>
#include "fdcan.h"

/**
 * @brief Initializes FDCAN strsuct with strandard and extended IDs
 * 
 * @param hcan pointer FDCAN handler
 * @param can pointer to FDCAN struct
 * @param standard_ids standard ID list
 * @param extended_ds extended ID lsist
 */
uint8_t can_filter_init(FDCAN_HandleTypeDef *hcan, can_t *can,			
    uint16_t standard_ids[2], uint32_t extended_ids[2]);

#endif
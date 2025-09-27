#ifndef _U_TX_CAN_H
#define _U_TX_CAN_H

#include "stm32h5xx.h"
#include <stdint.h>
#include "fdcan.h"

uint8_t can_filter_init(FDCAN_HandleTypeDef *hcan, can_t *can, uint16_t standard_ids[2], uint32_t extended_ids[2]);

#endif
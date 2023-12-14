#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"

typedef struct{
	CAN_HandleTypeDef *hcan;
	uint16_t *id_list;
	uint8_t id_list_len;

	/* desired behavior varies by app - so implement this at app level */
	void (*can_callback)(CAN_HandleTypeDef *hcan);
} can_t;

typedef struct{
	uint16_t id;
	uint8_t data[8];
	uint8_t len;
} can_msg_t;

/* function pointer type for the callback */
typedef void (*can_callback)(CAN_HandleTypeDef *hcan);

HAL_StatusTypeDef can_init(can_t *can, can_callback callback);
HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg);

#endif // CAN_H
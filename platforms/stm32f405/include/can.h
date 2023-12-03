#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct{
    CAN_HandleTypeDef *hcan;
    uint16_t *id_list;
    uint8_t size;
    HAL_CAN_RxFifo0MsgPendingCallback RxCallback;
} can_t;




#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"

void can_init(can_t *can);




#endif // CAN_H
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
    uint8_t size;

    /* desired behavior varies by app - so implement this at app level */
    void (*can_callback)(CAN_HandleTypeDef *hcan);
} can_t;

void can_init(can_t *can);
void can_send_msg(can_t *can, uint16_t id, uint8_t *data, uint8_t size);







#endif // CAN_H
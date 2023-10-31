#ifndef LTC4015_H_
#define LTC4015_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#define LTC4015_ADDR_68 (0x68 << 1) //I2C 7 bit address, which I believe means wants to shift the address. Also done in the other code
#define LTC4015_CHGSTATE = 0x34
typedef struct 
{
    I2C_HandleTypeDef *i2cHandle
    //Will hold the entire charge data, not useful currently but just want to keep the data in one place
    uint16_t CHGdata

}LTC4015_T

HAL_StatusTypeDef LTC4015_Init(LTC4015_T *dev, I2C_HandleTypeDef *i2cHandle)

HAL_StatusTypeDef LTC4015_read(LTC4015_T *dev, uint16_t reg, uint16_t *data)

HAL_StatusTypeDef LTC4015_write(LTC4015_T *dev, uint16_t reg, uint16_t data)

#ifndef MAX_NUM_LTC4015_INSTANCES
#define MAX_NUM_LTC4015_INSTANCES 1
#endif


#endif 

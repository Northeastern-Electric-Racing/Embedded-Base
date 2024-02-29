#ifndef M24C32_H
#define M24C32_H

#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

/* Write a word to an address of the M24C32 */
HAL_StatusTypeDef m24c32_write(I2C_HandleTypeDef *i2c_handle, uint16_t mem_address, uint8_t *data, uint16_t size);

/* Read a word from a memory address of the M24C32 */
HAL_StatusTypeDef m24c32_read(I2C_HandleTypeDef *i2c_handle, uint16_t mem_address, uint8_t *data, uint16_t size);

/* Clear memory from M24C32 */
HAL_StatusTypeDef m24c32_delete(I2C_HandleTypeDef *i2c_handle, uint16_t mem_address, uint16_t size);

#endif // M24C32_H
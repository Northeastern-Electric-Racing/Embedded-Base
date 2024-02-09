#ifndef I2CDUMP_H
#define I2CDUMP_H

#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx_hal_i2c.h"

/**
 * @brief Reads all the registers from the given peripheral 
 * 
 * @param hi2c
 * @param buffer
 * @param mode
 * @param start
 * @param end
 * 
 * @return int
 */
static int i2cdump(I2C_HandleTypeDef *hi2c, char** buffer, int mode, uint8_t start, uint8_t end);

#endif //I2CDUMP_H
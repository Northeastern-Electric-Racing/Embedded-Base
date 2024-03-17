#ifndef I2C_UTILITY_H
#define I2C_UTILITY_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEX_LABELS_H "\t 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n"
#define SPACING "  "

/**
 * @brief Probes all i2c addresses to check if devices are ready.
 * 
 * @param hi2c
 * @param buffer
 * @param mode
 * @param start
 * @param end
 * 
 * @return int
 */
int i2cdetect(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t start, uint8_t end);

/**
 * @brief Reads and dumps register data at a given register address and i2c bus.
 * 
 * @param hi2c
 * @param buffer
 * @param mode
 * @param offset
 * @param start
 * @param end
 */
int i2cdump(I2C_HandleTypeDef *hi2c, uint16_t devAddress, char **buffer, char mode, uint8_t start, uint8_t end);


#endif // I2C_UTILITY_H
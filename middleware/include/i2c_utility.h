#ifndef I2C_UTILITY_H
#define I2C_UTILITY_H

#include "serial_monitor.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEX_LABELS "\t 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n"
#define SPACING " "   // Spacing in between each printed address
#define MAX_TRIALS 3  // Max attempts to probe I2C device
#define TIMEOUT 50    // Max time until function skips
#define HEX 16        // hexadecimal for utoa function
#define BUF_ROWS 8    // Number of rows in the buffer
#define ROW_BYTES 128 // Number of bytes per row

/**
 * @brief Probes all i2c addresses to check if devices are ready.
 *
 * @param hi2c address of the i2c bus
 * @param buffer array to input the result in
 * @param mode
 * @param start
 * @param end
 *
 * @return int
 */
int i2cdetect(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t start,
              uint8_t end);

/**
 * @brief Reads and dumps register data at a given register address and i2c bus.
 *
 * @param hi2c address of the i2c bus
 * @param devAddress specific device address in the i2c bus to dump.
 * @param buffer array to input the result in
 * @param mode reading mode (default: byte)
 * @param start lower range of addresses to dump from
 * @param end higher range of addresses to dump from
 */
int i2cdump(I2C_HandleTypeDef *hi2c, uint16_t devAddress, char **buffer,
            char mode, uint8_t start, uint8_t end);

/**
 * @brief Prints the given 2D Array to serial monitor
 *
 * @param buffer
 */
void printResult(char **buffer);

#endif // I2C_UTILITY_H
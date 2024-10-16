/*
        PI4IOE5V9535ZDEX GPIO Expander Header File
        Link to part Datasheet for reference:
        https://www.diodes.com/assets/Datasheets/PI4IOE5V9535.pdf

        Author: Dylan Donahue
*/

#ifndef PI4IOE_H
#define PI4IOE_H

#include "stm32xx_hal.h"
#include <stdint.h>

typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint8_t port_config;

} pi4ioe_t;

/**
 * @brief Initializes the PI4IOE5V9535ZDEX GPIO Expander with current device
 * connections
 * @note needs update with any new device connections
 *
 * @param gpio
 * @param hi2c
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef pi4ioe_init(pi4ioe_t *gpio, I2C_HandleTypeDef *hi2c);

/**
 * @brief Writes to the PI4IOE5V9535ZDEX GPIO Expander
 *
 * @param device
 * @param val
 * @param i2c_handle
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef pi4ioe_write(uint8_t device, uint8_t val,
			       I2C_HandleTypeDef *i2c_handle);

/**
 * @brief Reads from the PI4IOE5V9535ZDEX GPIO Expander
 * @note always reads from both input registers
 *
 * @param buf
 * @param i2c_handle
 */
HAL_StatusTypeDef pi4ioe_read(uint8_t *buf, I2C_HandleTypeDef *i2c_handle);

#endif // PI4IOE_H
/**
 * @file max7314.h
 * @author Nick DePatie
 * @brief Driver for the MAX7314 GPIO Expander
 * @note Datasheet: https://www.mouser.com/datasheet/2/609/MAX7314-3130440.pdf
 * @version 0.1
 * @date 2023-12-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint8_t port_config;
} max7341_t;

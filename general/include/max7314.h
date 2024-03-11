#ifndef MAX7314_H
#define MAX7314_H

// Drivers for MAX7314AEG+T

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

extern I2C_HandleTypeDef *i2c_handle;

// i2c device address
extern uint16_t DEV_ADDR;

HAL_StatusTypeDef MAX7314_set_pin(uint8_t pin, uint8_t mode);

HAL_StatusTypeDef MAX7314_set_pins(uint8_t *pinConfigs, uint8_t reg);

HAL_StatusTypeDef MAX7314_read_pin(uint8_t pin, uint8_t *data);

HAL_StatusTypeDef MAX7314_read_pins(uint8_t reg, uint8_t *data);

HAL_StatusTypeDef MAX7314_set_pin_output(uint16_t pin, uint8_t state);

#endif // MAX7314_H
#ifndef MAX7314_H
#define MAX7314_H

/* Drivers for MAX7314AEG+T */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdbool.h>

typedef enum {
    MAX7314_PINS_0_TO_7 = 0,
    MAX7314_PINS_8_TO_15 = 1
} max7314_pin_regs_t;

typedef struct {
    
    I2C_HandleTypeDef *i2c_handle;

    /* i2c device address */
    uint16_t dev_addr;

} max7314_t;

void max7314_init(max7314_t *max, I2C_HandleTypeDef *i2c_handle);

/** 
 * @brief Write to the configuration register
 */

HAL_StatusTypeDef max7314_write_config(max7314_t *max, uint8_t *config);

/**
 * @brief Set the intensity of the output pins (low or high)
*/
HAL_StatusTypeDef max7314_set_global_intensity(max7314_t *max, uint8_t level);

/**
 * @brief Set pin to be an input or an output pin
 */
HAL_StatusTypeDef max7314_set_pin_mode(max7314_t *max, uint8_t pin, uint8_t mode);

HAL_StatusTypeDef max7314_set_pin_modes(max7314_t *max, uint8_t *pin_configs);

/** 
 * @brief Read an input pin
 */
HAL_StatusTypeDef max7314_read_pin(max7314_t *max, uint8_t pin, bool *state);

/** 
 * @brief Read an input pin
 */
HAL_StatusTypeDef max7314_read_pin(max7314_t *max, uint8_t pin, bool *state);

/**
 * @brief Turn an output pin on or off
 */
HAL_StatusTypeDef max7314_set_pin_state(max7314_t *max, uint8_t pin, bool state);

/** 
 * @brief Read the state of an output pin
 */
HAL_StatusTypeDef max7314_read_pin_state(max7314_t *max, uint8_t pin, bool* state);

#endif // MAX7314_H

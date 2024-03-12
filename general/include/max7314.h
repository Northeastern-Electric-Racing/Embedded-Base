#ifndef MAX7314_H
#define MAX7314_H

/* Drivers for MAX7314AEG+T */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#define MAX7314_INPUT_PORTS_0_TO_7  0x00
#define MAX7314_INPUT_PORTS_8_TO_15 0x01
#define MAX7314_PORT_CONFIG_0_TO_7  0x06
#define MAX7314_PORT_CONFIG_8_TO_15 0x07
#define MAX7314_CONFIG_REG          0x0F

enum max7314_pin_regs 
{
    MAX7314_PINS_0_TO_7 = 0;
    MAX7314_PINS_8_TO_15 = 1;
    MAX7314_ALL_PINS = 2;
};

enum max7314_pin_states {
    MAX7314_PIN_OFF = 0;
    MAX7314_PIN_ON = 1;
};

enum max7314_pin_modes {
    MAX7314_PIN_MODE_OUTPUT = 0;
    MAX7314_PIN_MODE_INPUT = 1;
};

typedef struct {
    
    I2C_HandleTypeDef *i2c_handle;

    /* i2c device address */
    uint16_t dev_addr;

} max7314_t;

HAL_StatusTypeDef max7314_init(max7314_t *max, I2C_HandleTypeDef *i2c_handle);

HAL_StatusTypeDef max7314_write_config(max7314_t *max, uint8_t *config);

/**
 * @brief Set pin to be an input or an output pin
 */
HAL_StatusTypeDef max7314_set_pin_mode(max7314_t *max, uint8_t pin, max7314_pin_modes mode);

HAL_StatusTypeDef max7314_set_pin_modes(max7314_t *max, max7314_pin_regs reg, uint8_t *pin_configs);

/** 
 * @brief Read an input pin
 */
HAL_StatusTypeDef max7314_read_pin(max7314_t *max, uint8_t pin, uint8_t *data);

HAL_StatusTypeDef max7314_read_pins(max7314_t *max, max7314_pin_regs reg, uint8_t *data);

/**
 * @brief Turn an output pin on or off
 */
HAL_StatusTypeDef max7314_set_pin_state(max7314_t *max, uint16_t pin, max7314_pin_states state);

/** 
 * @brief Read the state of an output pin
 */
HAL_StatusTypeDef max7314_read_pin_state(max7314_t *max, uint16_t pin, max7314_pin_states *state);

#endif // MAX7314_H
#ifndef TCA9539_H
#define TCA9539_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
TCA 9539 16 bit GPIO expander.  Datasheet:
https://www.ti.com/lit/ds/symlink/tca9539.pdf
*/

/// Possible I2C addresses, see comment below
///                            A0  A1
///    TCA_I2C_ADDR_0 0x74,    L   L
///    TCA_I2C_ADDR_1 0x75,    L   H
///    TCA_I2C_ADDR_2 0x76,    H   L
///    TCA_I2C_ADDR_3 0x77,    H   H
#define TCA_I2C_ADDR_0 0x74
#define TCA_I2C_ADDR_1 0x75
#define TCA_I2C_ADDR_2 0x76
#define TCA_I2C_ADDR_3 0x77

/// What to write/read to/from
/// 0 is for pins 0X, 1 is for pins 1X
/// INPUT: The incoming logic level (read only) Result: 1=H 0=L
/// OUTPUT: The outgoing logic level (write only) Set: 1=H 0=L
/// POLARITY: Inversion state, 1=Inverted 0=Uninverted
/// DIRECTION: Input/Output selection 1=Input 0=Output

#define TCA_INPUT_0_REG	    0x00 // Also change makefile eventually!
#define TCA_INPUT_1_REG	    0x01
#define TCA_OUTPUT_0_REG    0x02
#define TCA_OUTPUT_1_REG    0x03
#define TCA_POLARITY_0_REG  0x04
#define TCA_POLARITY_1_REG  0x05
#define TCA_DIRECTION_0_REG 0x06
#define TCA_DIRECTION_1_REG 0x07

typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint16_t dev_addr;
} tca9539_t;

/// Init TCA9539, a 16 bit I2C GPIO expander
void tca9539_init(tca9539_t *tca, I2C_HandleTypeDef *i2c_handle,
		  uint8_t dev_addr);

/// @brief Read all pins on a bus, for example using reg_type input to get
/// incoming logic level
HAL_StatusTypeDef tca9539_read_reg(tca9539_t *tca, uint8_t reg_type,
				   uint8_t *buf);
/// @brief Read a specific pin on a bus, do not iterate over this, use read_pins
/// instead
HAL_StatusTypeDef tca9539_read_pin(tca9539_t *tca, uint8_t reg_type,
				   uint8_t pin, uint8_t *buf);

/// @brief Write all pins on a bus, for example using reg_type OUTPUT to set
/// logic level or DIRECTION to set as output
HAL_StatusTypeDef tca9539_write_reg(tca9539_t *tca, uint8_t reg_type,
				    uint8_t buf);
/// @brief Write a specific pin on a bus, do not iterate over this, use
/// write_pins instead
HAL_StatusTypeDef tca9539_write_pin(tca9539_t *tca, uint8_t reg_type,
				    uint8_t pin, uint8_t buf);

#endif
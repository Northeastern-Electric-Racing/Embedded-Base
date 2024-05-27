#ifndef PCA9539_H
#define PCA9539_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
PCA 9539 16 bit GPIO expander.  Datasheet: https://www.ti.com/lit/ds/symlink/pca9539.pdf?ts=1716785085909
*/

/// Possible I2C addresses, see comment below
///                            A0  A1
///    PCA_I2C_ADDR_0 0x74,    L   L
///    PCA_I2C_ADDR_1 0x75,    L   H
///    PCA_I2C_ADDR_2 0x76,    H   L
///    PCA_I2C_ADDR_3 0x77,    H   H
#define PCA_I2C_ADDR_0 0x74
#define PCA_I2C_ADDR_1 0x75
#define PCA_I2C_ADDR_2 0x76
#define PCA_I2C_ADDR_3 0x77

/// What to write/read to/from
/// 0 is for pins 0X, 1 is for pins 1X
/// INPUT: The incoming logic level (read only) Result: 1=H 0=L
/// OUTPUT: The outgoing logic level (write only) Set: 1=H 0=L
/// POLARITY: Inversion state, 1=Inverted 0=Uninverted
/// DIRECTION: Input/Output selection 1=Input 0=Output

#define PCA_INPUT_0 0x00
#define PCA_INPUT_1 0x01
#define PCA_OUTPUT_0 0x02
#define PCA_OUTPUT_1 0x03
#define PCA_POLARITY_0 0x04
#define PCA_POLARITY_1 0x05
#define PCA_DIRECTION_0 0x06
#define PCA_DIRECTION_1 0x07

typedef struct
{
	I2C_HandleTypeDef *i2c_handle;
	uint16_t dev_addr;
} pca9539_t;

/// Init PCA9539, a 16 bit I2C GPIO expander
void pca9539_init(pca9539_t *pca, I2C_HandleTypeDef *i2c_handle, uint8_t dev_addr);

/// @brief Read all pins on a bus, for example using reg_type input to get incoming logic level
HAL_StatusTypeDef pca9539_read_reg(pca9539_t *pca, uint8_t reg_type,
									uint8_t config);
/// @brief Read a specific pin on a bus, do not iterate over this, use read_pins instead
HAL_StatusTypeDef pca9539_read_pin(pca9539_t *pca, uint8_t reg_type,
								   uint8_t pin, uint8_t *config);

/// @brief Write all pins on a bus, for example using reg_type OUTPUT to set logic level or DIRECTION to set as
/// output
HAL_StatusTypeDef pca9539_write_reg(pca9539_t *pca, uint8_t reg_type, uint8_t config);
/// @brief Write a specific pin on a bus, do not iterate over this, use write_pins instead
HAL_StatusTypeDef pca9539_write_pin(pca9539_t *pca, uint8_t reg_type, uint8_t pin,
									uint8_t config);

#endif
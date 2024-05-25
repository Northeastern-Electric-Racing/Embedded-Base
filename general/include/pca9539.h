#ifndef PCA9539_H
#define PCA9539_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

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

/// @brief Pin #, (0 through 7 on each bus, ex. bus 1 pin 3 is 13)
typedef enum {
	PCA_P0,
	PCA_P1,
	PCA_P2,
	PCA_P3,
	PCA_P4,
	PCA_P5,
	PCA_P6,
	PCA_P7,
} pca9539_pins_t;

/// @brief Bus #, Ex. P0 is B0 P0 and P12 is B1 P2
typedef enum { PCA_B0, PCA_B1 } pca9539_port_t;

/// What to write/read to/from
///
/// INPUT: The incoming logic level (read only) Result: 1=H 0=L
/// OUTPUT: The outgoing logic level (write only) Set: 1=H 0=L
/// POLARITY: Inversion state, 1=Inverted 0=Uninverted
/// DIRECTION: Input/Output selection 1=Input 0=Output
typedef enum {
	PCA_INPUT	  = 0,
	PCA_OUTPUT	  = 2,
	PCA_POLARITY  = 4,
	PCA_DIRECTION = 6,
} pca9539_reg_type_t;

typedef struct {
	I2C_HandleTypeDef* i2c_handle;
	uint16_t dev_addr;
} pca9539_t;

/// Init PCA9539, a 16 bit I2C GPIO expander
void pca9539_init(pca9539_t* pca, I2C_HandleTypeDef* i2c_handle, uint8_t dev_addr);

/// @brief Read all pins on a bus, using reg_type input to get incoming logic level
HAL_StatusTypeDef pca9539_read_pins(pca9539_t* pca, pca9539_reg_type_t reg_type,
									pca9539_port_t port, bool config[8]);
/// @brief Read a specific pin on a bus, do not iterate over this, use read_pins instead
HAL_StatusTypeDef pca9539_read_pin(pca9539_t* pca, pca9539_reg_type_t reg_type, pca9539_port_t port,
								   pca9539_pins_t pin, bool* config);

/// @brief Write all pins on a bus, using reg_type OUTPUT to set logic level or DIRECTION to set as
/// output
HAL_StatusTypeDef pca9539_write_pins(pca9539_t* pca, pca9539_reg_type_t reg_type,
									 pca9539_port_t port, bool config[8]);
/// @brief Write a specific pin on a bus, do not iterate over this, use write_pins instead
HAL_StatusTypeDef pca9539_write_pin(pca9539_t* pca, pca9539_reg_type_t reg_type,
									pca9539_port_t port, pca9539_pins_t pin,
									bool config);

#endif
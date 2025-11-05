#ifndef TCA9539_H
#define TCA9539_H

#include "stm32xx_hal.h"
#include <stdint.h>

/*
TCA9539QPWRQ1 16 bit GPIO expander.  Datasheet: https://www.ti.com/lit/gpn/tca9539-q1
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

#define TCA_INPUT_PORT_0	      0x00
#define TCA_INPUT_PORT_1	      0x01
#define TCA_OUTPUT_PORT_0	      0x02
#define TCA_OUTPUT_PORT_1	      0x03
#define TCA_POLARITY_INVERSION_PORT_0 0x04
#define TCA_POLARITY_INVERSION_PORT_1 0x05
#define TCA_CONFIGURATION_PORT_0      0x06
#define TCA_CONFIGURATION_PORT_1      0x07

/**
 * @brief Pointer to write function
 * 
 */
typedef uint8_t (*TCA_Write)(uint16_t dev_addr, uint8_t reg, uint8_t *data,
			     uint8_t size);

/**
 * @brief Pointer to read function
 * 
 */
typedef uint8_t (*TCA_Read)(uint16_t dev_addr, uint8_t reg, uint8_t *data,
			    uint8_t size);

typedef struct {
	TCA_Write write;
	TCA_Read read;

	uint16_t dev_addr;
} tca9539_t;

/**
 * @brief Initialize the TCA9539 Driver
 * 
 * @param tca pointer to a new driver struct
 * @param writeFunc function pointer to HAL specific write func
 * @param readFunc function pointer to HAL specific read func
 * @param dev_addr i2c device address
 */
void tca9539_init(tca9539_t *tca, TCA_Write writeFunc, TCA_Read readFunc,
		  uint8_t dev_addr);

/**
 * @brief Read the register of the TCA9539
 * 
 * @param tca pointer to driver struct with read & write funcs
 * @param reg_type type of register being read
 * @param buf pointer to buffer storing reg data
 * @return int Error code return
 */
int tca9539_read_reg(tca9539_t *tca, uint8_t reg_type, uint8_t *buf);

/**
 * @brief Read the pin state of the TCA9539
 * 
 * @param tca pointer to driver struct with read & write funcs
 * @param reg_type type of register being read
 * @param pin pin num to read
 * @param buf pointer storing buffer state
 * @return int Error code return
 */
int tca9539_read_pin(tca9539_t *tca, uint8_t reg_type, uint8_t pin,
		     uint8_t *buf);

/**

 * @brief Write the register of the TCA9539
 * 
 * @param tca pointer to driver struct with read & write funcs
 * @param reg_type type of register being written to 
 * @param buf pointer with value to write to reg
 * @return int Error code return
 */
int tca9539_write_reg(tca9539_t *tca, uint8_t reg_type, uint8_t buf);

/**
 * @brief Write the pin of the TCA9539
 * 
 * @param tca pointer to driver struct with read & write funcs
 * @param reg_type type of register being written to
 * @param pin pin to modify
 * @param buf pointer with value to write to reg
 * @return int Error code return
 */
int tca9539_write_pin(tca9539_t *tca, uint8_t reg_type, uint8_t pin,
		      uint8_t buf);

#endif
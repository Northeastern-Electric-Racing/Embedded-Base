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

#define PCA_INPUT_0_REG	    0x00
#define PCA_INPUT_1_REG	    0x01
#define PCA_OUTPUT_0_REG    0x02
#define PCA_OUTPUT_1_REG    0x03
#define PCA_POLARITY_0_REG  0x04
#define PCA_POLARITY_1_REG  0x05
#define PCA_DIRECTION_0_REG 0x06
#define PCA_DIRECTION_1_REG 0x07

/**
 * @brief Pointer to write function
 * 
 */
typedef uint8_t (*WritePtr)(uint16_t dev_addr, uint8_t reg, uint8_t *data,
			    uint8_t size);

/**
 * @brief Pointer to read function
 * 
 */
typedef uint8_t (*ReadPtr)(uint16_t dev_addr, uint8_t reg, uint8_t *data,
			   uint8_t size);

typedef struct {
	WritePtr write;
	ReadPtr read;

	uint16_t dev_addr;
} pca9539_t;

/**
 * @brief Initialize the PCA9539 Driver
 * 
 * @param pca pointer to a new driver struct
 * @param writeFunc function pointer to HAL specific write func
 * @param readFunc function pointer to HAL specific read func
 * @param dev_addr i2c device address
 */
void pca9539_init(pca9539_t *pca, WritePtr writeFunc, ReadPtr readFunc,
		  uint8_t dev_addr);

/**
 * @brief Read the register of the PCA9539
 * 
 * @param pca pointer to driver struct with read & write funcs
 * @param reg_type type of register being read
 * @param buf pointer to buffer storing reg data
 * @return int Error code return
 */
int pca9539_read_reg(pca9539_t *pca, uint8_t reg_type, uint8_t *buf);

/**
 * @brief Read the pin state of the PCA9539
 * 
 * @param pca pointer to driver struct with read & write funcs
 * @param reg_type type of register being read
 * @param pin pin num to read
 * @param buf pointer storing buffer state
 * @return int Error code return
 */
int pca9539_read_pin(pca9539_t *pca, uint8_t reg_type, uint8_t pin,
		     uint8_t *buf);

/**

 * @brief Write the register of the PCA9539
 * 
 * @param pca pointer to driver struct with read & write funcs
 * @param reg_type type of register being written to 
 * @param buf pointer with value to write to reg
 * @return int Error code return
 */
int pca9539_write_reg(pca9539_t *pca, uint8_t reg_type, uint8_t buf);

/**
 * @brief Write the pin of the PCA9539
 * 
 * @param pca pointer to driver struct with read & write funcs
 * @param reg_type type of register being written to
 * @param pin pin to modify
 * @param buf pointer with value to write to reg
 * @return int Error code return
 */
int pca9539_write_pin(pca9539_t *pca, uint8_t reg_type, uint8_t pin,
		      uint8_t buf);

#endif
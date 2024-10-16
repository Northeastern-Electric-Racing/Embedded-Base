#ifndef PCA9539_H
#define PCA9539_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
PCA 9539 16 bit GPIO expander.  Datasheet:
https://www.ti.com/lit/ds/symlink/pca9539.pdf?ts=1716785085909
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

//Function Pointer Initializiation, read/write functions will get assigned to these
typedef int(*I2C_WritePtr)( uint16_t address, uint8_t reg_type,
				uint8_t data );
typedef int(*I2C_ReadPtr)( uint16_t address, uint8_t reg_type,
			       uint8_t data );
//typedef void(*I2C_ReadPinFuncPtr){uint16_t };
//typedef void(*I2C_WritePinFuncPtr){};

typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint16_t dev_addr;

	// Two Function Pointers
	WritePtr Write;
	ReadPtr Read;
	//I2C_ReadPinFuncPtr local_I2C_Read_Pin;
	//I2C_WritePinFuncPtr local_I2C_Write_Pin;

} pca9539_t;

/// Init PCA9539, a 16 bit I2C GPIO expander
//Includes use of Function Pointers with instance writeFunc and readFunc
void pca9539_init(pca9539_t *pca, WritePtr writeFunc,
		  ReadPtr readFunc, I2C_HandleTypeDef *i2c_handle,
		  uint8_t dev_addr);

//READ/WRITE
//Don't need struct pointer from initialization?
//***The pointers for these functions will be general pca_read_reg, general pca_write_reg which are linked to HAL functions
int pca9539_read_reg(pca9539_t *pca, uint16_t reg_type, uint8_t *buf);

int pca9539_write_reg(pca9539_t *pca, uint16_t reg_type, uint8_t buf);

/*
/// @brief Read all pins on a bus, for example using reg_type input to get incoming logic level
HAL_StatusTypeDef pca9539_read_reg(pca9539_t *pca, uint8_t reg_type,
								   uint8_t *buf);

/// @brief Write all pins on a bus, for example using reg_type OUTPUT to set logic level or DIRECTION to set as
/// output
HAL_StatusTypeDef pca9539_write_reg(pca9539_t *pca, uint8_t reg_type, uint8_t buf);
*/

//PIN WRITE/READ

/// @brief Write a specific pin on a bus, do not iterate over this, use write_pins instead
//HAL_StatusTypeDef
int pca9539_write_pin(pca9539_t *pca, uint16_t reg_type, uint8_t pin,
		      uint8_t buf);
/// @brief Read a specific pin on a bus, do not iterate over this, use read_pins instead
//HAL_StatusTypeDef
int pca9539_read_pin(pca9539_t *pca, uint16_t reg_type, uint8_t pin,
		     uint8_t *buf);

#endif
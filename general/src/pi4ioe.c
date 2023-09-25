/*
	PI4IOE5V9535ZDEX GPIO Expander Source File
	Link to part Datasheet for reference:
	https://www.diodes.com/assets/Datasheets/PI4IOE5V9535.pdf

	Author: Dylan Donahue
*/
#include "pi4ioe.h"

	uint8_t outputReg1;
	uint8_t outputReg2;

HAL_StatusTypeDef pi4ioe_init(pi4ioe_t *gpio, I2C_HandleTypeDef *i2c_handle)
{
	gpio->i2c_handle = i2c_handle;
	gpio->port_config = IO_CONFIG_BUF;

	outputReg1 = 0x00;
	outputReg2 = 0x00;

	uint8_t buf[2] = {IO_CONFIG_REG, IO_CONFIG_BUF};

	/* write to config reg setting TSMS to input, rest to output */
	return HAL_I2C_Master_Transmit(i2c_handle, PI4IOE_I2C_ADDR, buf, 2, HAL_MAX_DELAY);
}

 HAL_StatusTypeDef pi4ioe_write(uint8_t device, uint8_t val, I2C_HandleTypeDef *i2c_handle)
 {

	if (device > 7) outputReg1 = OUTPUT1_REG;
	else outputReg1 = OUTPUT0_REG;

	uint8_t buf[2] = {outputReg1, (outputReg2 | val << device)};
	return HAL_I2C_Master_Transmit(i2c_handle, PI4IOE_I2C_ADDR, buf, 2, HAL_MAX_DELAY);

 }

  HAL_StatusTypeDef pi4ioe_read(uint8_t *buf, I2C_HandleTypeDef *i2c_handle)
  {
	uint8_t reg = INPUT0_REG;

	HAL_I2C_Master_Transmit(i2c_handle, PI4IOE_I2C_ADDR, &reg, 1, HAL_MAX_DELAY);
	return HAL_I2C_Master_Receive(i2c_handle, PI4IOE_I2C_ADDR, buf, 2, HAL_MAX_DELAY);

  }

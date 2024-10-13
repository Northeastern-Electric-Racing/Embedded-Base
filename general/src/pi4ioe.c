/*
        PI4IOE5V9535ZDEX GPIO Expander Source File
        Link to part Datasheet for reference:
        https://www.diodes.com/assets/Datasheets/PI4IOE5V9535.pdf

        Author: Dylan Donahue
*/
#include "pi4ioe.h"

// all of these addresses are complete bullshit and wrong plz fix when know :)
// and remove this comment the values should be correct though, just the order
// is wrong

#define PI4IOE_I2C_ADDR 0x20

#define IO_CONFIG_REG 0x06
#define IO_CONFIG_BUF 0x10 /* = 0001 0000, bit 4 = TSMS = input */
#define INPUT0_REG    0x00
#define INPUT1_REG    0x01
#define OUTPUT0_REG   0x02
#define OUTPUT1_REG   0x03

uint8_t oreg_1_data;
uint8_t oreg_2_data;

HAL_StatusTypeDef pi4ioe_init(pi4ioe_t *gpio, I2C_HandleTypeDef *i2c_handle)
{
	gpio->i2c_handle = i2c_handle;
	gpio->port_config = IO_CONFIG_BUF;

	oreg_1_data = 0x00;
	oreg_2_data = 0x00;

	uint8_t buf[2] = { IO_CONFIG_REG, IO_CONFIG_BUF };

	/* write to config reg setting TSMS to input, rest to output */
	return HAL_I2C_Master_Transmit(i2c_handle, PI4IOE_I2C_ADDR, buf, 2,
				       HAL_MAX_DELAY);
}

HAL_StatusTypeDef pi4ioe_write(uint8_t device, uint8_t val,
			       I2C_HandleTypeDef *i2c_handle)
{
	uint8_t reg;
	uint8_t *oreg_data;

	if (device > 7) {
		reg = OUTPUT1_REG;
		oreg_data = &oreg_1_data;
	} else {
		reg = OUTPUT0_REG;
		oreg_data = &oreg_2_data;
	}

	*oreg_data |= val << device;

	uint8_t buf[2] = { reg, *oreg_data };

	return HAL_I2C_Master_Transmit(i2c_handle, PI4IOE_I2C_ADDR, buf, 2,
				       HAL_MAX_DELAY);
}

HAL_StatusTypeDef pi4ioe_read(uint8_t *buf, I2C_HandleTypeDef *i2c_handle)
{
	uint8_t reg = INPUT0_REG;

	HAL_I2C_Master_Transmit(i2c_handle, PI4IOE_I2C_ADDR, &reg, 1,
				HAL_MAX_DELAY);
	return HAL_I2C_Master_Receive(i2c_handle, PI4IOE_I2C_ADDR, buf, 2,
				      HAL_MAX_DELAY);
}

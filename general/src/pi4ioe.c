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

typedef (*I2C_WriteFuncPtr)(uint16_t device_addr, uint8_t *data, uint16_t size);
typedef (*I2C_ReadFuncPtr)(uint16_t device_addr, uint8_t *reg,
			   uint16_t reg_size, uint8_t *data,
			   uint16_t data_size);

typedef struct {
	uint16_t i2c_addr; // I2C address of the GPIO expander
	uint8_t port_config; // Port configuration buffer
	uint8_t oreg_1_data; // Output register 1 data
	uint8_t oreg_2_data; // Output register 2 data

	I2C_WriteFuncPtr i2c_write; // Function pointer for I2C write operation
	I2C_ReadFuncPtr i2c_read; // Function pointer for I2C read operation
} pi4ioe_t;

int pi4ioe_init(pi4ioe_t *gpio, I2C_WriteFuncPtr i2c_write, uint16_t i2c_addr)
{
	gpio->i2c_addr = i2c_addr;
	gpio->port_config = IO_CONFIG_BUF;
	gpio->oreg_1_data = 0x00;
	gpio->oreg_2_data = 0x00;

	gpio->i2c_write = i2c_write;

	uint8_t buf[2] = { IO_CONFIG_REG, IO_CONFIG_BUF };

	/* Write to the configuration register using the provided I2C write function */
	return gpio->i2c_write(gpio->i2c_addr, buf, 2);
}

int pi4ioe_write(pi4ioe_t *gpio, uint8_t device, uint8_t val)
{
	uint8_t reg;
	uint8_t *oreg_data;

	if (device > 7) {
		reg = OUTPUT1_REG;
		oreg_data = &gpio->oreg_1_data;
	} else {
		reg = OUTPUT0_REG;
		oreg_data = &gpio->oreg_2_data;
	}

	*oreg_data |= val << device;

	uint8_t buf[2] = { reg, *oreg_data };

	/* Use the generalized I2C write function */
	return gpio->i2c_write(gpio->i2c_addr, buf, 2);
}

int pi4ioe_read(pi4ioe_t *gpio, uint8_t *buf)
{
	uint8_t reg = INPUT0_REG;

	/* Use the generalized I2C read function */
	return gpio->i2c_read(gpio->i2c_addr, &reg, 1, buf, 2);
}

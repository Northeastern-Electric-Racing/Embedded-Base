/*
	MCP23008 GPIO Expander Source File
	Link to part Datasheet for reference:
	https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf

	Author: Dylan Donahue
*/

#include "mcp23008.h"

#define MCP23008_I2C_ADDR   0x20 
#define IO_CONFIG_REG 		0x00
#define IO_CONFIG_OUTPUT 	0x00 /* = 0000 0000, all output */
#define IO_CONFIG_INPUT 	0xFF /* = 1111 1111, all input */
#define INPUT_REG 			0x09

int mcp23008_init(mcp23008_t *obj, int (*write_func)(uint8_t, const uint8_t*, uint8_t), int (*read_func)(uint8_t, uint8_t*, uint8_t))
{
    obj->write = write_func;
    obj->read = read_func;

    uint8_t buf[2] = {IO_CONFIG_REG, IO_CONFIG_OUTPUT};
    return obj->write(MCP23008_I2C_ADDR, buf, 2);
}

int mcp23008_write_reg(mcp23008_t *obj, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return obj->write(MCP23008_I2C_ADDR, buf, 2);
}

int mcp23008_read_reg(mcp23008_t *obj, uint8_t reg, uint8_t *val)
{
    return obj->read(MCP23008_I2C_ADDR, val, 1);
}
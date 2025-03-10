#include "tca9539.h"

#include "../../middleware/include/c_utils.h"

#define REG_SIZE_BITS 8

static int tca_write_reg(tca9539_t *tca, uint8_t address, uint8_t *data)
{
	return tca->write(tca->dev_addr, address, data, 1);
}

static int tca_read_reg(tca9539_t *tca, uint8_t address, uint8_t *data)
{
	return tca->read(tca->dev_addr, address, data, 1);
}

void tca9539_init(tca9539_t *tca, TCA_Write writeFunc, TCA_Read readFunc,
		  uint8_t dev_addr)
{
	tca->dev_addr = dev_addr << 1u;

	tca->write = writeFunc;
	tca->read = readFunc;
}

int tca9539_read_reg(tca9539_t *tca, uint8_t reg_type, uint8_t *buf)
{
	int status = tca_read_reg(tca, reg_type, buf);
	if (status) {
		return status;
	}

	return status;
}

int tca9539_read_pin(tca9539_t *tca, uint8_t reg_type, uint8_t pin,
		     uint8_t *buf)
{
	uint8_t data;
	int status = tca_read_reg(tca, reg_type, &data);
	if (status) {
		return status;
	}

	*buf = (data & (1 << pin)) > 0;

	return status;
}

int tca9539_write_reg(tca9539_t *tca, uint8_t reg_type, uint8_t buf)
{
	return tca_write_reg(tca, reg_type, &buf);
}

int tca9539_write_pin(tca9539_t *tca, uint8_t reg_type, uint8_t pin,
		      uint8_t buf)
{
	uint8_t data;
	uint8_t data_new;

	int status = tca_read_reg(tca, reg_type, &data);
	if (status) {
		return status;
	}

	data_new = (data & ~(1u << pin)) | (buf << pin);

	return tca_write_reg(tca, reg_type, &data_new);
}

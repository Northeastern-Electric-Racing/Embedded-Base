#include "pca9539.h"

#include "../../middleware/include/c_utils.h"

#define REG_SIZE_BITS 8

static int pca_write_reg(pca9539_t *pca, uint8_t address, uint8_t *data)
{
	return pca->write(pca->dev_addr, address, data, 1);
}

static int pca_read_reg(pca9539_t *pca, uint8_t address, uint8_t *data)
{
	return pca->read(pca->dev_addr, address, data, 1);
}

void pca9539_init(pca9539_t *pca, PCA_Write writeFunc, PCA_Read readFunc,
		  uint8_t dev_addr)
{
	pca->dev_addr = dev_addr << 1u;

	pca->write = writeFunc;
	pca->read = readFunc;
}

int pca9539_read_reg(pca9539_t *pca, uint8_t reg_type, uint8_t *buf)
{
	int status = pca_read_reg(pca, reg_type, buf);
	if (status) {
		return status;
	}

	return status;
}

int pca9539_read_pin(pca9539_t *pca, uint8_t reg_type, uint8_t pin,
		     uint8_t *buf)
{
	uint8_t data;
	int status = pca_read_reg(pca, reg_type, &data);
	if (status) {
		return status;
	}

	*buf = (data & (1 << pin)) > 0;

	return status;
}

int pca9539_write_reg(pca9539_t *pca, uint8_t reg_type, uint8_t buf)
{
	return pca_write_reg(pca, reg_type, &buf);
}

int pca9539_write_pin(pca9539_t *pca, uint8_t reg_type, uint8_t pin,
		      uint8_t buf)
{
	uint8_t data;
	uint8_t data_new;

	int status = pca_read_reg(pca, reg_type, &data);
	if (status) {
		return status;
	}

	data_new = (data & ~(1u << pin)) | (buf << pin);

	return pca_write_reg(pca, reg_type, &data_new);
}
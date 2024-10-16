#include "pca9539.h"

#include "../../middleware/include/c_utils.h"

#define REG_SIZE_BITS 8


void pca9539_init(pca9539_t *pca, I2C_WriteFuncPtr writeFunc,
		  I2C_ReadFuncPtr readFunc, I2C_HandleTypeDef *i2c_handle,
		  uint8_t dev_addr)
{
	pca->i2c_handle = i2c_handle;
	pca->dev_addr = dev_addr << 1u;
}


int pca9539_read_reg(pca9539_t *pca, uint16_t reg_type, uint8_t *buf)
{
	return pca->local_I2C_Read(pca->dev_addr, reg_type, *buf);
}

int pca9539_write_reg(pca9539_t *pca, uint16_t reg_type, uint8_t buf)
{
	return pca->local_I2C_Write(pca->dev_addr, reg_type, buf);
}

int pca9539_write_pin(pca9539_t *pca, uint16_t reg_type, uint8_t pin,
		      uint8_t buf)
{
	//uint8_t data; -> where is this used?
	

	int status = pca->local_I2C_Write(pca->dev_addr, reg_type, buf);
	if (status) {
		return status;
	}
	//uint8_t data_new;
	//uint8_t data;
	//data_new = (data & ~(1u << pin)) | (buf << pin);

	return pca->local_I2C_Write(pca->dev_addr, reg_type, buf);
}
int pca9539_read_pin(pca9539_t *pca, uint16_t reg_type, uint8_t pin,
		     uint8_t *buf)
{
	//uint8_t data;

	int status = pca->local_I2C_Read(pca->dev_addr, reg_type, *buf);
	if (status) {
		return status;
	}
	//*buf = (data & (1 << pin)) > 0; What to do with this?

	return pca->local_I2C_Read(pca->dev_addr, reg_type, *buf);
}

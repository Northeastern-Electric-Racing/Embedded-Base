#include "pca9539.h"

#include "../../middleware/include/c_utils.h"

#define REG_SIZE_BITS 8

HAL_StatusTypeDef pca_write_reg(pca9539_t *pca, uint16_t address, uint8_t *data)
{
	// ensure shifting left one, HAL adds the write bit
	return HAL_I2C_Mem_Write(pca->i2c_handle, pca->dev_addr, address,
				 I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef pca_read_reg(pca9539_t *pca, uint16_t address, uint8_t *data)
{
	return HAL_I2C_Mem_Read(pca->i2c_handle, pca->dev_addr, address,
				I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

void pca9539_init(pca9539_t *pca, I2C_HandleTypeDef *i2c_handle,
		  uint8_t dev_addr)
{
	pca->i2c_handle = i2c_handle;
	pca->dev_addr = dev_addr
			<< 1u; /* shifted one to the left cuz STM says so */
}

HAL_StatusTypeDef pca9539_read_reg(pca9539_t *pca, uint8_t reg_type,
				   uint8_t *buf)
{
	HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, buf);
	if (status) {
		return status;
	}

	return status;
}

HAL_StatusTypeDef pca9539_read_pin(pca9539_t *pca, uint8_t reg_type,
				   uint8_t pin, uint8_t *buf)
{
	uint8_t data;
	HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, &data);
	if (status) {
		return status;
	}

	*buf = (data & (1 << pin)) > 0;

	return status;
}

HAL_StatusTypeDef pca9539_write_reg(pca9539_t *pca, uint8_t reg_type,
				    uint8_t buf)
{
	return pca_write_reg(pca, reg_type, &buf);
}

HAL_StatusTypeDef pca9539_write_pin(pca9539_t *pca, uint8_t reg_type,
				    uint8_t pin, uint8_t buf)
{
	uint8_t data;
	uint8_t data_new;

	HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, &data);
	if (status) {
		return status;
	}

	data_new = (data & ~(1u << pin)) | (buf << pin);

	return pca_write_reg(pca, reg_type, &data_new);
}

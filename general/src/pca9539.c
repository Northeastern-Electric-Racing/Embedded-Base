#include "pca9539.h"

#define REG_SIZE_BITS 8
#define TIMEOUT		  HAL_MAX_DELAY

HAL_StatusTypeDef pca_write_reg(pca9539_t* pca, uint16_t address, uint8_t* data)
{
	// ensure shifting left one, HAL adds the write bit
	return HAL_I2C_Mem_Write(pca->i2c_handle, pca->dev_addr << 1, address, I2C_MEMADD_SIZE_8BIT,
							 data, sizeof(data), TIMEOUT);
}

HAL_StatusTypeDef pca_read_reg(pca9539_t* pca, uint16_t address, uint8_t* data)
{

	return HAL_I2C_Mem_Read(pca->i2c_handle, pca->dev_addr << 1, address, I2C_MEMADD_SIZE_8BIT,
							data, sizeof(data), TIMEOUT);
}

void pca9539_init(pca9539_t* pca, I2C_HandleTypeDef* i2c_handle, uint8_t dev_addr)
{
	pca->i2c_handle = i2c_handle;
	pca->dev_addr	= dev_addr << 1u; /* shifted one to the left cuz STM says so */
}

HAL_StatusTypeDef pca9539_read_reg(pca9539_t* pca, uint8_t reg_type, uint8_t config)
{

	HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, &config);
	if (status) {
		return status;
	}

	return status;
}

HAL_StatusTypeDef pca9539_read_pin(pca9539_t* pca, uint8_t reg_type, uint8_t pin, uint8_t* config)
{
	uint8_t data;
	HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, &data);
	if (status) {
		return status;
	}

	*config = (data & (1 << pin)) > 0;

	return status;
}

HAL_StatusTypeDef pca9539_write_reg(pca9539_t* pca, uint8_t reg_type, uint8_t config)
{

	return pca_write_reg(pca, reg_type, &config);
}

HAL_StatusTypeDef pca9539_write_pin(pca9539_t* pca, uint8_t reg_type, uint8_t pin, uint8_t config)
{

	uint8_t data;

	HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, &data);
	if (status) {
		return status;
	}

	uint8_t data_new = (data & ~(1u << pin)) | (config << pin);

	return pca_write_reg(pca, reg_type, &data_new);
}

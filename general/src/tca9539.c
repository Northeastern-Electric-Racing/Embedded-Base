#include "tca9539.h"

#include "../../middleware/include/c_utils.h"

#define REG_SIZE_BITS 8

HAL_StatusTypeDef tca_write_reg(tca9539_t *tca, uint16_t address, uint8_t *data)
{
	// ensure shifting left one, HAL adds the write bit
	return HAL_I2C_Mem_Write(tca->i2c_handle, tca->dev_addr, address,
				 I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef tca_read_reg(tca9539_t *tca, uint16_t address, uint8_t *data)
{
	return HAL_I2C_Mem_Read(tca->i2c_handle, tca->dev_addr, address,
				I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

void tca9539_init(tca9539_t *tca, I2C_HandleTypeDef *i2c_handle,
		  uint8_t dev_addr)
{
	tca->i2c_handle = i2c_handle;
	tca->dev_addr = dev_addr
			<< 1u; /* shifted one to the left cuz STM says so */
}

HAL_StatusTypeDef tca9539_read_reg(tca9539_t *tca, uint8_t reg_type,
				   uint8_t *buf)
{
	HAL_StatusTypeDef status = tca_read_reg(tca, reg_type, buf);
	if (status) {
		return status;
	}

	return status;
}

HAL_StatusTypeDef tca9539_read_pin(tca9539_t *tca, uint8_t reg_type,
				   uint8_t pin, uint8_t *buf)
{
	uint8_t data;
	HAL_StatusTypeDef status = tca_read_reg(tca, reg_type, &data);
	if (status) {
		return status;
	}

	*buf = (data & (1 << pin)) > 0;

	return status;
}

HAL_StatusTypeDef tca9539_write_reg(tca9539_t *tca, uint8_t reg_type,
				    uint8_t buf)
{
	return tca_write_reg(tca, reg_type, &buf);
}

HAL_StatusTypeDef tca9539_write_pin(tca9539_t *tca, uint8_t reg_type,
				    uint8_t pin, uint8_t buf)
{
	uint8_t data;
	uint8_t data_new;

	HAL_StatusTypeDef status = tca_read_reg(tca, reg_type, &data);
	if (status) {
		return status;
	}

	data_new = (data & ~(1u << pin)) | (buf << pin);

	return tca_write_reg(tca, reg_type, &data_new);
}

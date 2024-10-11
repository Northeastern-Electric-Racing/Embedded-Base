#include "m24c32.h"

uint16_t check_size(uint16_t size)
{
	if (size < M24C32_PAGE_SIZE)
		return size;
	else
		return M24C32_PAGE_SIZE;
}

HAL_StatusTypeDef eeprom_write(uint16_t mem_address, uint8_t *data,
			       uint16_t size)
{
	HAL_StatusTypeDef status;

	for (int bytes_written = 0; bytes_written < size; bytes_written += 32) {
		uint16_t write_size = check_size(size - bytes_written);

		status = HAL_I2C_Mem_Write(i2c_handle, M24C32_I2C_ADDR,
					   mem_address + bytes_written, 2,
					   &data[bytes_written], write_size,
					   1000);
		if (status)
			return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef eeprom_read(uint16_t mem_address, uint8_t *data,
			      uint16_t size)
{
	HAL_StatusTypeDef status;

	for (int bytes_read = 0; bytes_read < size; bytes_read += 32) {
		uint16_t read_size = check_size(size - bytes_read);

		status = HAL_I2C_Mem_Read(i2c_handle, M24C32_I2C_ADDR,
					  mem_address + bytes_read, 2,
					  &data[bytes_read], read_size, 1000);
		if (status)
			return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef eeprom_delete(uint16_t mem_address, uint16_t size)
{
	HAL_StatusTypeDef status;

	// Create a list of zeroes that will be written to the EEPROM
	uint8_t data[size];
	memset(data, 0, size);

	for (int bytes_deleted = 0; bytes_deleted < size; bytes_deleted += 32) {
		uint16_t delete_size = check_size(size - bytes_deleted);

		// Write 0 to memory addresses
		status = HAL_I2C_Mem_Write(i2c_handle, M24C32_I2C_ADDR,
					   mem_address + bytes_deleted, 2,
					   &data[bytes_deleted], delete_size,
					   1000);
		if (status)
			return status;
	}

	return HAL_OK;
}
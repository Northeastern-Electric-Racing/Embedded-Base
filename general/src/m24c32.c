#include "m24c32.h"

#define PAGE_SIZE 32 /* Bytes */
#define SUCCESS	  0

/**
 * @brief Clamp a write length to the size of the m24c32 page size.
 * 
 * @param size The length of the buffer to write, in Bytes.
 * @return uint16_t The amount of bytes to write in a write cycle.
 */
uint16_t clamp_to_page(uint16_t size)
{
	if (size < PAGE_SIZE)
		return size;
	else
		return PAGE_SIZE;
}

int m24c32_write(m24c32_t *device, uint16_t addr, uint8_t *data, uint16_t len)
{
	int status;

	/* Always use a page write */
	for (int bytes_written = 0; bytes_written < size; bytes_written += 32) {
		uint16_t write_len = clamp_to_page(len);

		int result = device->write(addr, data, write_len);

		/* Let application handle errors */
		if (result)
			return result;

		len -= bytes_written;
	}

	return SUCCESS;
}

int m24c32_read(m32c32_t *device, uint16_t mem_address, uint8_t *data,
		uint16_t len)
{
	return device->read();
}

int eeprom_delete(m32c32_t *device, uint16_t mem_address, uint16_t len)
{
	uint8_t data[len];
	memset(data, 0, len);

	return device->write(mem_address, data, len);
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

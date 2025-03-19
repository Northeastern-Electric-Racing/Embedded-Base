#include "m24c32.h"
#include <stdlib.h>

#define PAGE_SIZE 32 /* Bytes */

/**
 * @brief Clamp a write length to the size of the m24c32 page size.
 * 
 * @param size The length of the buffer to write, in Bytes.
 * @return uint16_t The amount of bytes to write in a write cycle.
 */
static uint16_t clamp_to_page(uint16_t size)
{
	if (size < PAGE_SIZE)
		return size;
	else
		return PAGE_SIZE;
}

eeprom_status_t m24c32_write(m24c32_t *device, uint16_t addr, uint8_t *data,
			     uint16_t len)
{
	if ((device == NULL) || (device->write == NULL) || (data == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	eeprom_status_t result = EEPROM_OK;

	/* Always use a page write */
	for (size_t bytes_written = 0; bytes_written < len;
	     bytes_written += PAGE_SIZE) {
		uint16_t write_len = clamp_to_page(len - bytes_written);

		result = device->write(addr + bytes_written,
				       &data[bytes_written], write_len);

		/* Let application handle errors */
		if (result != EEPROM_OK) {
			return result;
		}
	}

	return result;
}

eeprom_status_t m24c32_read(m24c32_t *device, uint16_t mem_address,
			    uint8_t *data, uint16_t len)
{
	if ((device == NULL) || (device->read == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	return device->read(mem_address, data, len);
}

eeprom_status_t m24c32_clear(m24c32_t *device, uint16_t mem_address,
			     uint16_t len)
{
	if ((device == NULL) || (device->write == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	uint8_t *data = (uint8_t *)calloc(len, sizeof(uint8_t));

	if (data == NULL) {
		return EEPROM_ERROR_ALLOCATION;
	}

	eeprom_status_t result = device->write(mem_address, data, len);

	free(data);

	return result;
}
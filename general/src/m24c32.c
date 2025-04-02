#include "m24c32.h"
#include <stdlib.h>

#define PAGE_SIZE 32 /* Bytes */

eeprom_status_t m24c32_write(m24c32_t *device, uint16_t addr, uint8_t *data,
			     uint16_t len)
{
	if ((device == NULL) || (device->write == NULL) || (data == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	eeprom_status_t result = EEPROM_OK;
	size_t bytes_written = 0;

	while (bytes_written < len) {
		uint16_t remaining_in_page = PAGE_SIZE - (addr % PAGE_SIZE);
		uint16_t write_len = (len - bytes_written < remaining_in_page) ?
					     (len - bytes_written) :
					     remaining_in_page;

		result = device->write(addr, &data[bytes_written], write_len);
		if (result != EEPROM_OK) {
			return result;
		}

		addr += write_len;
		bytes_written += write_len;
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
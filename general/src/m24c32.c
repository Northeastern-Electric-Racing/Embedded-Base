#include "m24c32.h"

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

int m24c32_write(m24c32_t *device, uint16_t addr, uint8_t *data, uint16_t len)
{
	int result;

	/* Always use a page write */
	for (int bytes_written = 0; bytes_written < len;
	     bytes_written += PAGE_SIZE) {
		uint16_t write_len = clamp_to_page(len);

		result = device->write(addr, data, write_len);

		/* Let application handle errors */
		if (result)
			return result;

		len -= bytes_written;
		addr += PAGE_SIZE;
	}

	return result;
}

int m24c32_read(m24c32_t *device, uint16_t mem_address, uint8_t *data,
		uint16_t len)
{
	return device->read(mem_address, data, len);
}

int m24c32_clear(m24c32_t *device, uint16_t mem_address, uint16_t len)
{
	uint8_t data[len];
	memset(data, 0, len);

	return device->write(mem_address, data, len);
}

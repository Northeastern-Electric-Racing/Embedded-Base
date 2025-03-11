#include "m24c32_eeprom_directory.h"

int m24c32_directory_write(const eeprom_directory_t *directory,
			   m24c32_t *eeprom, const char *id, uint8_t *data,
			   size_t len, size_t offset)
{
	int32_t base = eeprom_get_base_address(directory, id);
	if (base < 0) {
		return -1; // Invalid ID
	}

	int32_t size = eeprom_get_size(directory, id);

	if (offset >= size) {
		return -2; // Invalid Offset
	}

	if (offset + len > size) {
		return -3; // Overwrting next partition
	}

	return m24c32_write(eeprom, base + offset, data, len);
}

int m24c32_directory_read(const eeprom_directory_t *directory, m24c32_t *eeprom,
			  const char *id, uint8_t *data, size_t len,
			  size_t offset)
{
	int32_t base = eeprom_get_base_address(directory, id);

	if (base < 0) {
		return -1; // Invalid ID
	}

	int32_t size = eeprom_get_size(directory, id);

	if (offset >= size) {
		return -2; // Invalid Offset
	}

	if (offset + len > size) {
		return -3; // Reading beyond current partition
	}

	return m24c32_read(eeprom, base + offset, data, len);
}
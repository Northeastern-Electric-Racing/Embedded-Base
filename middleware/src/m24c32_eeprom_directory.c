#include <m24c32_eeprom_directory.h>

int m24c32_directory_write(const struct eeprom_directory *directory,
			   m24c32_t *eeprom, char *id, uint8_t *data,
			   size_t len, size_t offset)
{
	int32_t base = eeprom_get_base_address(directory, id);
	if (base < 0) {
		return -1;
	}

	int32_t size = eeprom_get_size(directory, id);

	/* Overwriting into next partition */
	if (base + offset + len > base + size) {
		return -2;
	}

	return m24c32_write(eeprom, base + offset, data, len);
}

int m24c32_directory_read(m24c32_t *eeprom, char *id, uint8_t data, size_t len,
			  size_t offset)
{
	int32_t base = eeprom_get_base_address(directory, id);
	if (base < 0) {
		return -1;
	}

	int32_t size = eeprom_get_size(directory, id);

	return m24c32_write(eeprom, base + offset, data, len);
}
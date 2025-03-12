#include "m24c32_eeprom_directory.h"

eeprom_status_t m24c32_directory_write(const eeprom_directory_t *directory,
				       m24c32_t *eeprom, const char *id,
				       uint8_t *data, uint16_t len,
				       uint16_t offset)
{
	if ((directory == NULL) || (eeprom == NULL) || (id == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	uint16_t base;
	eeprom_status_t status = eeprom_get_base_address(directory, id, &base);

	if (status != EEPROM_OK) {
		return status; // Invalid ID
	}

	uint16_t size;
	status = eeprom_get_size(directory, id, &size);
	if (status != EEPROM_OK) {
		return status;
	}

	if (offset >= size) {
		return EEPROM_ERROR_OUT_OF_BOUNDS; // Invalid Offset
	}

	if (offset + len > size) {
		return EEPROM_ERROR_ALIGNMENT; // Overwrting next partition
	}

	return m24c32_write(eeprom, base + offset, data, len);
}

eeprom_status_t m24c32_directory_read(const eeprom_directory_t *directory,
				      m24c32_t *eeprom, const char *id,
				      uint8_t *data, uint16_t len,
				      uint16_t offset)
{
	if ((directory == NULL) || (eeprom == NULL) || (id == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	uint16_t base;
	eeprom_status_t status = eeprom_get_base_address(directory, id, &base);
	if (status != EEPROM_OK) {
		return status; // Invalid ID
	}

	uint16_t size;
	status = eeprom_get_size(directory, id, &size);
	if (status != EEPROM_OK) {
		return status;
	}

	if (offset >= size) {
		return EEPROM_ERROR_OUT_OF_BOUNDS; // Invalid Offset
	}

	if (offset + len > size) {
		return EEPROM_ERROR_ALIGNMENT; // Reading beyond current partition
	}

	return m24c32_read(eeprom, base + offset, data, len);
}
/**
 * @file m24c32.h
 * @brief EEPROM Read/Write Interface for M24C32
 * 
 * This file defines the interface for interacting with the M24C32 EEPROM.
 * Functions return `eeprom_status_t` error codes, which are standardized in `eeprom_status.h`.
 * 
 */

#ifndef M24C32_H
#define M24C32_H

#include "stm32xx_hal.h"
#include "eeprom_status.h"
#include <stdint.h>
#include <string.h>

typedef eeprom_status_t (*write_ptr)(uint16_t addr, uint8_t *data,
				     uint16_t len);
typedef eeprom_status_t (*read_ptr)(uint16_t addr, uint8_t *data, uint16_t len);

typedef struct {
	write_ptr write;
	read_ptr read;
} m24c32_t;

/**
 * @brief Write to the m24c32 EEPROM.
 * 
 * This function performs a paged write operation to the EEPROM.
 * 
 * @param device I2C read and write function pointers.
 * @param addr Memory address where data should be written.
 * @param data Buffer of bytes to write.
 * @param len The amount of bytes to write.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code on failure.
 */
eeprom_status_t m24c32_write(m24c32_t *device, uint16_t addr, uint8_t *data,
			     uint16_t len);

/**
 * @brief Read from the m24c32 EEPROM.
 * 
 * This function retrieves data starting from the specified address and stores it 
 * in the provided buffer.
 * 
 * @param device I2C read and write function pointers.
 * @param addr Memory address from which data should be read.
 * @param data Buffer where data will be written to.
 * @param len The amount of bytes to read.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code on failure.
 */
eeprom_status_t m24c32_read(m24c32_t *device, uint16_t addr, uint8_t *data,
			    uint16_t len);

/**
 * @brief Clear bytes in the EEPROM.
 * 
 * This function writes zeroes to the specified memory section to reset its contents.
 * 
 * @param device I2C read and write function pointers.
 * @param addr Memory address of the write head.
 * @param len Number of bytes to clear.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code on failure.
 */
eeprom_status_t m24c32_clear(m24c32_t *device, uint16_t addr, uint16_t len);

#endif // M24C32_H
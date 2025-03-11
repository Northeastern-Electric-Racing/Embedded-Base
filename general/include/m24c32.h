#ifndef M24C32_H
#define M24C32_H

#include "stm32xx_hal.h"
#include <stdint.h>
#include <string.h>

typedef int (*write_ptr)(uint16_t addr, const uint8_t *data, uint16_t len);
typedef int (*read_ptr)(uint16_t addr, uint8_t *data, uint16_t len);

typedef struct {
	write_ptr write;
	read_ptr read;
} m24c32_t;

/**
 * @brief Write to the m24c32 EEPROM.
 * 
 * @param device I2C read and write function pointers.
 * @param addr Memory address of the write head.
 * @param data Buffer of bytes to write.
 * @param len The amount of bytes to write.
 * @return int Error code: 0 on success, nonzero on failure.
 */
int m24c32_write(m24c32_t *device, uint16_t addr, uint8_t *data, uint16_t len);

/**
 * @brief Read from the m24c32 EEPROM.
 * 
 * @param device I2C read and write function pointers.
 * @param addr Memory address of the read head.
 * @param data Buffer where data will be written to.
 * @param len The amount of bytes to read.
 * @return int Error code: 0 on success, nonzero on failure.
 */
int m24c32_read(m24c32_t *device, uint16_t addr, uint8_t *data, uint16_t len);

/**
 * @brief Clear bytes in the EEPROM.
 * 
 * @param device I2C read and write function pointers.
 * @param addr Memory address of the write head.
 * @param len Number of bytes to clear.
 * @return int Error code: 0 on success, nonzero on failure.
 */
int m24c32_clear(m24c32_t *device, uint16_t addr, uint16_t len);

#endif // M24C32_H
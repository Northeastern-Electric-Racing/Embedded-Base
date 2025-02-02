#ifndef M24C32_H
#define M24C32_H

#include "stm32xx_hal.h"
#include <stdint.h>
#include <string.h>

typedef int (*write_ptr)(uint8_t addr, const uint8_t *data, uint8_t len);
typedef int (*read_ptr)(uint8_t addr, uint8_t *data, uint8_t len);
typedef struct {
	write_ptr write;
	read_ptr read;
} m24c32_t;

int m24c32_write(m32c32_t *device, uint16_t addr, uint8_t *data, uint16_t len);

int m24c32_read(m32c32_t *device, uint16_t addr, uint8_t *data, uint16_t len);

int eeprom_delete(m32c32_t *device, uint16_t addr, uint16_t len);

#endif // M24C32_H
#ifndef hdc2021debr_h
#define hdc2021debr_h
#include <stdbool.h>
#include <stdint.h>
/**
 * https://www.ti.com/lit/ds/symlink/hdc2021.pdf
 * --Datasheet
 * 
 */

#define HDC2021_I2C_ADDR \
	0x40 /* If ADDR is connected to VDD, 0x41  This is shifted left as it is required to be read as a dev address.*/

typedef enum {
    REG_TEMP_LOW = 0x00,
    REG_TEMP_HIGH = 0x01,
    REG_HUMID_LOW = 0x02,
    REG_HUMID_HIGH = 0x03,
	REG_STATUS = 0x04,
	REG_DEVICE_CONFIG = 0x0E,
    REG_MEASURE_CONFIG = 0x0F
} HDC2021_REG_ADDR_t;

/** Function Pointers */
typedef int (*Write_ptr)(uint8_t *data, uint8_t dev_address, uint8_t length);
typedef int (*Read_ptr)(uint8_t *data, uint8_t reg, uint8_t dev_address,
			uint8_t length);

typedef struct {
	Write_ptr write_reg;
	Read_ptr read_reg;
	float temp;
	float humidity;
	bool is_heater_enabled;
	uint8_t dev_address;
} hdc2021debr_t;

/**
 * @brief Initializes an HDC2021 Driver
 *
 * @param hdc2021 - hdc2021debr driver
 * @return int - Status code
 */
int hdc2021_init(hdc2021debr_t *hdc2021, Write_ptr write_reg, Read_ptr read_reg,
		   uint8_t dev_address);
/**
 * @brief Toggles the status of the internal heater
 *
 * @param hdc2021 - hdc2021debr driver
 * @param enable - true to enable, false to disable
 * @return int - Status code
 */
int hdc2021_toggle_heater(hdc2021debr_t *hdc2021, bool enable);
/**
 * @brief Retrieves the temperature and humidity
 * @note Call hdc2021_trigger_oneshot() and wait a couple ms before calling this
 * @param hdc2021 - hdc2021debr driver
 * @return int - Status code
 */
int hdc2021_get_temp_humid(hdc2021debr_t *hdc2021);
#endif
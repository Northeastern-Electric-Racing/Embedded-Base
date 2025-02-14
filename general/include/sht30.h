#ifndef sht30_h
#define sht30_h

#include <stdbool.h>
#include <stdint.h>

/**
 * https://www.mouser.com/datasheet/2/682/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital-971521.pdf
 * --Datasheet
 *
 */
#define SHT30_I2C_ADDR \
	0x44 << 1u /* If ADDR (pin2) is connected to VDD, 0x45  \
                                   */

typedef enum {
	SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH = 0x2c06,
	SHT3X_COMMAND_CLEAR_STATUS = 0x3041,
	SHT3X_COMMAND_SOFT_RESET = 0x30A2,
	SHT3X_COMMAND_HEATER_ENABLE = 0x306d,
	SHT3X_COMMAND_HEATER_DISABLE = 0x3066,
	SHT3X_COMMAND_READ_STATUS = 0xf32d,
	SHT3X_COMMAND_FETCH_DATA = 0xe000,
	SHT3X_COMMAND_MEASURE_HIGHREP_10HZ = 0x2737,
	SHT3X_COMMAND_MEASURE_LOWREP_10HZ = 0x272a
} sht3x_command_t;

/** Function Pointers */
typedef int (*Write_ptr)(uint8_t *data, uint8_t dev_address, uint8_t reg, uint8_t length);
typedef int (*Read_ptr)(uint8_t *data, uint8_t dev_address, uint8_t reg, uint8_t length);

/*
 * Start measurement command with clock streching enabled and high
 * repeatability. This is responsible for retrieving the temp and humidity in
 * single shot mode
 */
#define SHT30_START_CMD_WCS 0x2C06

/* Start measurement command with clock streching disabled and high
 * repeatability */
#define SHT30_START_CMD_NCS 0x2400

typedef struct {
	Write_ptr write_reg;
	Read_ptr read_reg;
	float temp;
	float humidity;
	bool is_heater_enabled;
	uint8_t dev_address;
} sht30_t;

/**
 * @brief Initializes an SHT30 Driver
 *
 * @param sht30 - SHT30 driver
 * @return int - Status code
 */
int sht30_init(sht30_t *sht30, Write_ptr write_reg, Read_ptr read_reg, uint8_t dev_address);

/**
 * @brief Toggles the status of the internal heater
 *
 * @param sht30 - SHT30 driver
 * @param enable - true to enable, false to disable
 * @return int - Status code
 */
int sht30_toggle_heater(sht30_t *sht30, bool enable);

/**
 * @brief Retrieves the temperature and humidity
 *
 * @param sht30 - SHT30 driver
 * @return int - Status code
 */
int sht30_get_temp_humid(sht30_t *sht30);

#endif
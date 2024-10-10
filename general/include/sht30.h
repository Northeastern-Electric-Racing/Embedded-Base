#ifndef SHT30_H
#define SHT30_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * https://www.mouser.com/datasheet/2/682/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital-971521.pdf
 * --Datasheet
 *
 */                                   
#define SHT30_I2C_ADDR (0x44u << 1u) /* If ADDR (pin2) is connected to VDD, 0x45 */

typedef enum {
  SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH = 0x2c06u,
  SHT3X_COMMAND_CLEAR_STATUS = 0x3041u,
  SHT3X_COMMAND_SOFT_RESET = 0x30A2u,
  SHT3X_COMMAND_HEATER_ENABLE = 0x306du,
  SHT3X_COMMAND_HEATER_DISABLE = 0x3066u,
  SHT3X_COMMAND_READ_STATUS = 0xf32du,
  SHT3X_COMMAND_FETCH_DATA = 0xe000u,
  SHT3X_COMMAND_MEASURE_HIGHREP_10HZ = 0x2737u,
  SHT3X_COMMAND_MEASURE_LOWREP_10HZ = 0x272au
} sht3x_command_t;

/*
 * Start measurement command with clock streching enabled and high
 * repeatability. This is responsible for retrieving the temp and humidity in
 * single shot mode
 */
#define SHT30_START_CMD_WCS 0x2C06

/* Start measurement command with clock streching disabled and high
 * repeatability */
#define SHT30_START_CMD_NCS 0x2400

// Function pointer types
typedef int (*I2C_TransmitFunc)(uint8_t addr, const uint8_t *data, size_t len, uint32_t timeout);
typedef int (*I2C_ReceiveFunc)(uint8_t addr, uint8_t *data, size_t len, uint32_t timeout);
typedef int (*I2C_MemReadFunc)(uint8_t addr, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t *data, size_t len, uint32_t timeout);
typedef void (*DelayFunc)(uint32_t ms);


typedef struct {
  I2C_TransmitFunc i2c_transmit;
  I2C_ReceiveFunc i2c_receive;
  I2C_MemReadFunc i2c_mem_read;
  DelayFunc delay;
  uint16_t temp;
  uint16_t humidity;
} sht30_t;

/**
 * @brief Initializes an SHT30 Driver
 *
 * @param sht30 - SHT30 driver
 * @return int - 0 on success, negative value on error
 */
int sht30_init(sht30_t *sht30);

/**
 * @brief Toggles the status of the internal heater
 *
 * @param sht30 - SHT30 driver
 * @param enable - true to enable, false to disable
 * @return int - 0 on success, negative value on error
 */
int sht30_toggle_heater(sht30_t *sht30, bool enable);

/**
 * @brief Retrieves the temperature and humidity
 *
 * @param sht30 - SHT30 driver
 * @return int - 0 on success, negative value on error
 */
int sht30_get_temp_humid(sht30_t *sht30);

#endif 
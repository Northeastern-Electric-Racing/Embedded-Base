#ifndef ADS131M04_H
#define ADS131M04_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

typedef struct
{
  SPI_HandleTypeDef *spi;
  GPIO_TypeDef *gpio;
  uint8_t cs_pin;

} ads_config;

#define NULL_CMD 0x0
#define RESET_CMD 0x11
#define STANDBY_CMD 0x22
#define WAKEUP_CMD 0x33
#define LOCK_CMD 0x555
#define UNLOCK_CMD 0x655

// Method to initialize communication over SPI and
void ADS131M04_initialize(ltc_config *conf, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio, uint8_t cs_pin);

// Method to abstract sending a command to SPI
void ADS131M04_send_command(uint8_t cmd);

// Method to abstract writing to a register, will use SPI commands under the hood
void ADS131M04_write_reg(uint8_t reg, uint8_t value);

// Method to abstract reading from a register, will use SPI commands under the hood to do
uint8_t ADS131M04_read_reg(uint8_t reg);

// should read values out of the ADC
uint8_t ADS131M04_read_ADC();
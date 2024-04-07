#ifndef ADS131M04_H
#define ADS131M04_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

typedef struct
{
  SPI_HandleTypeDef *spi;
  GPIO_TypeDef *gpio;
  uint8_t cs_pin;

} ads131_t;

/*  Method to initialize communication over SPI and configure the ADC into Continuous Conversion Mode*/
void ads131m04_initialize(ltc_config *conf, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio, uint8_t cs_pin);

/* Method to abstract sending a command to SPI */
void ads131m04_send_command(uint8_t cmd);

/* Method to abstract writing to a register, will use SPI commands under the hood */
void ads131m04_write_reg(uint8_t reg, uint8_t value);

/* Method to abstract reading from a register, will use SPI commands under the hood to do */
uint8_t ads131m04_read_reg(uint8_t reg);

/* Method to read values out of the ADC */
uint8_t ads131m04_read_ADC();

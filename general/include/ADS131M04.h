#ifndef ADS131M04_H
#define ADS131M04_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

#define NULL_CMD 0x0
#define RESET_CMD 0x11
#define STANDBY_CMD 0x22
#define WAKEUP_CMD 0x33
#define LOCK_CMD 0x555
#define UNLOCK_CMD 0x655

void ADS131M04_initialize(ltc_config *conf, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio, uint8_t cs_pin);

void ADS131M04_send_command(uint8_t cmd);

void ADS131M04_write_reg(uint8_t reg, uint8_t value);

uint8_t ADS131M04_read_reg(uint8_t reg);
#include "ADS131M04.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define NULL_CMD 0x0
#define RESET_CMD 0x11
#define STANDBY_CMD 0x22
#define WAKEUP_CMD 0x33
#define LOCK_CMD 0x555
#define UNLOCK_CMD 0x655

/*  Method to initialize communication over SPI and configure the ADC into Continuous Conversion Mode*/
void ads131m04_initialize(ads131_t *conf, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio,
                          uint8_t cs_pin)
{
    conf->spi = hspi;
    conf->gpio = hgpio;
    conf->cs_pin = cs_pin;

    HAL_GPIO_WritePin(conf->gpio, conf->cs_pin, GPIO_PIN_SET);

    // Add code for configurations

    ads131m04_send_command(conf, RESET_CMD);

    return conf;
}

/* Method to abstract sending a command to SPI */
void ads131m04_send_command(ads131_t *config, uint8_t cmd)
{
    HAL_GPIO_WritePin(config->gpio, config->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(config->spi, cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(config->gpio, config->cs_pin, GPIO_PIN_SET);
}

/* Method to abstract writing to a register, will use SPI commands under the hood */
void ads131m04_write_reg(uint8_t reg, uint8_t value)
{
}

/* Method to abstract reading from a register, will use SPI commands under the hood to do */
uint8_t ads131m04_read_reg(uint8_t reg)
{
}

/* Method to read values out of the ADC */
uint8_t ads131m04_read_ADC()
{
}

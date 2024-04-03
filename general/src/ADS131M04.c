#include "ADS131M04.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void ADS131M04_initialize(ads_config *conf, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio,
                          uint8_t cs_pin)
{
    conf->spi = hspi;
    conf->gpio = hgpio;
    conf->cs_pin = cs_pin;

    HAL_GPIO_WritePin(conf->gpio, conf->cs_pin, GPIO_PIN_SET);

    // Add code for configurations

    ADS131M04_send_command(conf, RESET_CMD);

    return conf;
}

// Function to send command over SPI
void ADS131M04_send_command(ads_config *config, uint8_t cmd)
{
    HAL_GPIO_WritePin(config->gpio, config->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(config->spi, cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(config->gpio, config->cs_pin, GPIO_PIN_SET);
}

// Method to abstract writing to a register, will use SPI commands under the hood
void ADS131M04_write_reg(uint8_t reg, uint8_t value)
{
}

// Method to abstract reading from a register, will use SPI commands under the hood to do
uint8_t ADS131M04_read_reg(uint8_t reg)
{
}

// should read values out of the ADC
uint8_t ADS131M04_read_ADC()
{
}
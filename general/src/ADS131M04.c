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
void ads131m04_initialize(ads131_t *adc, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio,
                          uint8_t cs_pin)
{
    adc->spi = hspi;
    adc->gpio = hgpio;
    adc->cs_pin = cs_pin;

    HAL_GPIO_WritePin(adc->gpio, adc->cs_pin, GPIO_PIN_SET);

    // Channel 0 has Gain default set to level 1, which is what we want for this application
    // We can try out different configurations later if we need to, but it seems like the defaults work

    return adc;
}

/* Method to abstract sending a command to SPI */
void ads131m04_send_command(ads131_t *adc, uint16_t cmd)
{
    HAL_GPIO_WritePin(adc->gpio, adc->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(adc->spi, cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(adc->gpio, adc->cs_pin, GPIO_PIN_SET);
}

/* Method to abstract writing to a register, will use SPI commands under the hood, value is MSB aligned */
void ads131m04_write_reg(ads131_t *adc, uint8_t reg, uint16_t value)
{
    // Ensure register address is within 6-bit range
    reg &= 0x3F;

    // Set the number of registers we're writing to to 1 for now
    uint8_t num_registers = 1;

    // Construct SPI word
    uint16_t spi_word = 0;
    spi_word |= (0x3000);            // 011 in bits 13-15
    spi_word |= (reg << 7);          // Register address (bits 7-12)
    spi_word |= (num_registers - 1); // Number of registers (bits 0-6)

    // Send command to write to the specified register immediately followed by sending the value we want to write to that register
    ads131m04_send_command(adc, spi_word);
    ads131m04_send_command(adc, value);
}

/* Method to abstract reading from a register, will use SPI commands under the hood to do */
uint16_t ads131m04_read_reg(ads131_t *adc, uint8_t reg)
{
    // Ensure register address is within 6-bit range
    reg &= 0x3F;
    // Read one register at a time for now
    uint8_t num_registers = 0;

    // Construct SPI word
    uint16_t spi_word = 0;
    spi_word |= (0x5000);            // 101 in bits 13-15
    spi_word |= (reg << 7);          // Register address (bits 7-12)
    spi_word |= (num_registers - 1); // Number of registers (bits 0-6)

    ads131m04_send_command(adc, spi_word);

    uint16_t res = 0;

    if (HAL_SPI_Receive(adc->hspi, (uint16_t *)&res, 1, 10) != HAL_OK)
        return 1;

    return res;
}

/* Method to read values out of the ADC, should be called immediately after the DRDY interrupt is triggered */
uint16_t ads131m04_read_ADC(ads131_t *adc)
{
    uint16_t status_register = 0;
    uint16_t data_register = 0;

    // Read the status register
    if (HAL_SPI_Receive(adc->hspi, (uint8_t *)&status_register, 2, 10) != HAL_OK)
        return 1;

    // Read the data register
    if (HAL_SPI_Receive(adc->hspi, (uint8_t *)&data_register, 2, 10) != HAL_OK)
        return 1;

    return data_register;
}

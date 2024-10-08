#ifndef ADS131M04_H
#define ADS131M04_H

#include "stm32xx_hal.h"

typedef struct {
  SPI_HandleTypeDef *spi;
  GPIO_TypeDef *gpio;
  uint8_t cs_pin;
} ads131_t;

/*  Method to initialize communication over SPI and configure the ADC into
 * Continuous Conversion Mode*/
void ads131m04_init(ads131_t *adc, SPI_HandleTypeDef *hspi, GPIO_TypeDef *hgpio,
                    uint8_t cs_pin);

/* Method to read values out of the ADC */
HAL_StatusTypeDef ads131m04_read_adc(ads131_t *adc, uint32_t *adc_values);

#endif /* ADS131M04_H */

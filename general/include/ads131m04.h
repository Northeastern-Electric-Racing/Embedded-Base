#ifndef ADS131M04_H
#define ADS131M04_H

#include "stm32xx_hal.h"

/* Function Pointers*/
typedef void (*GPIO_WritePtr)(uint8_t pin, uint8_t PinState);
typedef void (*WritePtr)(uint8_t *pData, uint16_t size);
typedef int (*ReadPtr)(uint8_t *pData, uint16_t size);

typedef struct {
	uint8_t pin;
	GPIO_WritePtr gpio_write;
	WritePtr write;
	ReadPtr read;
} ads131_t;

/*  Method to initialize communication over SPI and configure the ADC into
 * Continuous Conversion Mode*/
void ads131m04_init(ads131_t *adc, GPIO_WritePtr gpio_write, WritePtr write,
		    ReadPtr read);

/* Method to read values out of the ADC */
int ads131m04_read_adc(ads131_t *adc, uint32_t *adc_values);

#endif /* ADS131M04_H */

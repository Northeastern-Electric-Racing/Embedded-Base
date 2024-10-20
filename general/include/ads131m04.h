#ifndef ADS131M04_H
#define ADS131M04_H

#include "stm32xx_hal.h"

/* Function Pointers*/
typedef void (*GPIO_WriteFuncPtr)(uint8_t cs_pin, uint8_t PinState);
typedef void (*SPI_TransmitFuncPtr)(uint8_t *pData, uint16_t size);
typedef int (*SPI_ReceiveFuncPtr)(uint8_t *pData, uint16_t size);

typedef struct {
	uint8_t cs_pin;
	GPIO_WriteFuncPtr local_GPIO_Write;
	SPI_TransmitFuncPtr local_SPI_Transmit;
	SPI_ReceiveFuncPtr local_SPI_Receive;
} ads131_t;

/*  Method to initialize communication over SPI and configure the ADC into
 * Continuous Conversion Mode*/
void ads131m04_init(ads131_t *adc, GPIO_WriteFuncPtr local_GPIO_write,
		    SPI_TransmitFuncPtr local_SPI_transmit,
		    SPI_ReceiveFuncPtr local_SPI_receive);

/* Method to read values out of the ADC */
int ads131m04_read_adc(ads131_t *adc, uint32_t *adc_values);

#endif /* ADS131M04_H */

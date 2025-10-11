#include "as3935.h"

/**
 * @brief initializes attributes for as3935_t struct; also makes sure cs pin is high
 * 
 * @param as3935 pointer to the allocated as3935_t struct
 * @param hspi pointer to the SPI_HandleTypeDef of the sensor
 * @param cs_port port of the cs line in spi
 * @param cs_pin pin of the cs line in spi
 */
void as3935_init(as3935_t *as3935, SPI_HandleTypeDef *hspi,
		 GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
	as3935->hspi = hspi;
	as3935->cs_port = cs_port;
	as3935->cs_pin = cs_pin;
	as3935->address = 0;

	HAL_GPIO_WritePin(as3935->cs_port, as3935->cs_pin, GPIO_PIN_SET);
}

/**
 * @brief reads from the lightning sensor from the given address
 * 
 * @param as3935 the pointer to the as3935_t struct containing sensor information
 * @param reg the register you wish to access
 * @return returns the read value or 0xFF if the operation failed
 */
uint8_t as3935_read(as3935_t *as3935, uint8_t reg)
{
	// first two bits of tx_data is be 0x01. Next 6 bits should be register address.
	uint16_t tx_data = AS3935_READ_CMD | ((reg & 0b111111) << 8);
	uint8_t rx_buf[2] = { 0 };

	HAL_GPIO_WritePin(as3935->cs_port, as3935->cs_pin, GPIO_PIN_RESET);
	// HAL SPI wants an 8 bit array of length 2 for tx_data
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(
		as3935->hspi, (uint8_t *)&tx_data, &rx_buf, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(as3935->cs_port, as3935->cs_pin, GPIO_PIN_SET);

	if (status != HAL_OK) {
		return 0xFF;
	}

	return rx_buf[1];
}

/**
 * @brief writes to the lightning sensor with the given address
 * 
 * @param as3935 the pointer to the as3935_t struct containing sensor information
 * @param reg the register you wish to write to
 * @return returns whether the operation was successful
 */
uint8_t as3935_write(as3935_t *as3935, uint8_t reg, uint8_t value)
{
	uint16_t tx_data = AS3935_WRITE_CMD | ((reg & 0b111111) << 8) | value;

	HAL_GPIO_WritePin(as3935->cs_port, as3935->cs_pin, GPIO_PIN_RESET);
	// HAL SPI wants an 8 bit array of length 2 for tx_data
	HAL_StatusTypeDef status = HAL_SPI_Transmit(
		as3935->hspi, (uint8_t *)&tx_data, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(as3935->cs_port, as3935->cs_pin, GPIO_PIN_SET);

	return status == HAL_OK;
}

/**
 * @brief resets settings for lightning sensor
 * 
 * @param as3935 the pointer to the as3935_t object containing sensor information
 * @return returns whether the operation was successful
 */
uint8_t as3935_reset(as3935_t *as3935)
{
	// performs reset command
	return as3935_write(as3935, AS3935_PRESET_DEFAULT, 0x96);
}

/**
 * @brief calibrates the internal RC Oscillators
 * 
 * @param as3935 the pointer to the as3935_t object containing sensor information
 * @return returns whether the operation was successful
 */
uint8_t as3935_calibrate_RCO(as3935_t *as3935)
{
	// performs calibrate command
	return as3935_write(as3935, AS3935_CALIB_RCO, 0x96);
}

/**
 * @brief set afe value (either AS3935_AFE_INDOOR for indoor detection 
 *        or AS3935_AFE_OUTDOOR for outdoor detection)
 * 
 * @param as3935 the pointer to the as3935_t object containing sensor information
 * @return returns whether the operation was successful
 */
uint8_t as3935_set_AFE(as3935_t *as3935, uint8_t afe_setting)
{
	// read current register value
	uint8_t current_reg = as3935_read(as3935, AS3935_AFE_GB);

	if (current_reg == 0xFF) {
		return HAL_ERROR;
	}

	// clear AFE_GB bits 5-1 and set it to new afe_setting value
	current_reg &= 0b11000001; // clear bits 5-1
	current_reg |= (afe_setting << 1);

	return as3935_write(as3935, AS3935_AFE_GB, current_reg);
}

/**
 * @brief returns the the lightning sensor interupt value 
 *        (one of: AS3935_INT_NH, AS3935_INT_D, AS3935_INT_L, or 0)
 * 
 * @param as3935 the pointer to the as3935_t object containing sensor information
 * @return the value of the interrupt register
 */
uint8_t as3935_get_interrupt(as3935_t *as3935)
{
	// Read interrupt register and return bits 3 to 0
	uint8_t int_reg = as3935_read(as3935, AS3935_INT);

	if (int_reg == 0xFF) {
		return HAL_ERROR;
	}

	// return only interrupt bits
	return int_reg & 0b00001111;
}

/**
 * @brief returns distance of lightning in kilometers 
 *        (or AS3935_DISTANCE_OUT_OF_RANGE or AS3935_DISTANCE_OVERHEAD)
 * 
 * @param as3935 the pointer to the as3935_t object containing sensor information
 * @return the distance in kilometers or AS3935_DISTANCE_OUT_OF_RANGE or AS3935_DISTANCE_OVERHEAD
 */
uint8_t as3935_get_distance(as3935_t *as3935)
{
	// read distance register and return bits 5 to 0
	uint8_t dist_reg = as3935_read(as3935, AS3935_DISTANCE);

	if (dist_reg == 0xFF) {
		return HAL_ERROR;
	}

	// read first 5 bits
	return dist_reg & 0b111111;
}

/**
 * @brief returns lightning energy
 * 
 * @param as3935 the pointer to the as3935_t object containing sensor information
 * @return the lightning energy
 */
uint32_t as3935_get_energy(as3935_t *as3935)
{
	uint32_t energy = 0;

	// read all three energy registers
	uint8_t energy_mm = as3935_read(as3935, AS3935_S_LIG_MM);
	uint8_t energy_m = as3935_read(as3935, AS3935_S_LIG_M);
	uint8_t energy_l = as3935_read(as3935, AS3935_S_LIG_L);

	if (energy_mm == 0xFF || energy_l == 0xFF || energy_m == 0xFF) {
		return HAL_ERROR;
	}

	// combine the three values. Only read the first 5 bits for energy_mm
	energy = ((energy_mm & 0b11111) << 16) | (energy_m << 8) | energy_l;

	return energy;
}
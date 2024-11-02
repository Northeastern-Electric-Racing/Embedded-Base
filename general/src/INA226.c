/*
        INA226AQDGSRQ1 Current Sensor I2C Driver
        Datasheet:
        https://www.ti.com/lit/ds/symlink/ina226-q1.pdf
*/

#include "INA226.h"

void ina226_init(ina226_t *ina, WritePtr write, ReadPtr read, uint16_t dev_addr)
{
	ina->write = write;
	ina->read = read;
	ina->dev_addr = dev_addr << 1u;
	ina->current_lsb = 0;
}

int ina226_read_reg(ina226_t *ina, uint8_t reg, uint16_t *data)
{
	return ina->read(ina->dev_addr, reg, data);
}

int ina226_write_reg(ina226_t *ina, uint8_t reg, uint16_t *data)
{
	return ina->write(ina->dev_addr, reg, data);
}

// Writes calibration register. r_shunt in ohms, max_current in amps
int ina226_calibrate(ina226_t *ina, float r_shunt, float max_current)
{
	float current_lsb = max_current / 32768;
	float cal = 0.00512 / (current_lsb * r_shunt);
	uint16_t cal_reg = (uint16_t)floorf(cal);
	ina->current_lsb = 0.00512 / (cal_reg * r_shunt);

	return ina226_write_reg(ina, INA226_CALIBRATION, &cal_reg);
}

// Reads current in amps
int ina226_read_current(ina226_t *ina, float *data)
{
	uint16_t current_reg;

	int status = ina226_read_reg(ina, INA226_CURRENT, &current_reg);
	if (status != 0) {
		return status;
	}

	*data = (float)(int16_t)current_reg * ina->current_lsb;

	return status;
}

// Reads power in watts
int ina226_read_power(ina226_t *ina, float *data)
{
	uint16_t power_reg;

	int status = ina226_read_reg(ina, INA226_POWER, &power_reg);
	if (status != 0) {
		return status;
	}

	*data = (float)(int16_t)power_reg * (ina->current_lsb * 25);

	return status;
}

// Reads shunt voltage in volts
int ina226_read_shunt_voltage(ina226_t *ina, float *data)
{
	uint16_t shunt_voltage_reg;

	int status =
		ina226_read_reg(ina, INA226_SHUNT_VOLTAGE, &shunt_voltage_reg);
	if (status != 0) {
		return status;
	}

	*data = (float)(int16_t)shunt_voltage_reg *
		2.5e-6; // LSB = 2.5 uV per bit

	return status;
}

// Reads bus voltage in volts
int ina226_read_bus_voltage(ina226_t *ina, float *data)
{
	uint16_t bus_voltage_reg;

	int status = ina226_read_reg(ina, INA226_BUS_VOLTAGE, &bus_voltage_reg);
	if (status != 0) {
		return status;
	}

	*data = (float)bus_voltage_reg * 1.25e-3; // LSB = 1.25 mV per bit

	return status;
}

// Sets configuration register bits 0-11 (operating mode, shunt voltage conversion time, bus voltage conversion time, and averaging mode)
// See datasheet for settings
int ina226_configure(ina226_t *ina, uint8_t mode, uint8_t vshct, uint8_t vbusct,
		     uint8_t avg)
{
	uint16_t configuration;
	configuration = (avg << 9) | (vbusct << 6) | (vshct << 3) | mode;
	return ina226_write_reg(ina, INA226_CONFIGURATION, &configuration);
}

// Resets all registers to default values
int ina226_reset(ina226_t ina)
{
	uint16_t reset = INA226_CONFIG_RESET_MASK;
	return ina226_write_reg(ina, INA226_CONFIGURATION, &reset);
}

// Reads manufacturer id register
int ina226_read_manufacturer_id(ina226_t ina, uint16_t *data)
{
	uint16_t manufacturer_id;

	int status =
		ina226_read_reg(ina, INA226_MANUFACTURER, &manufacturer_id);
	if (status != 0) {
		return status;
	}

	*data = manufacturer_id;

	return status;
}

// Reads die id
int ina226_read_die_id(ina226_t ina, uint16_t *data)
{
	uint16_t die_id;

	int status = ina226_read_reg(ina, INA226_DIE_ID, &die_id);
	if (status != 0) {
		return status;
	}

	*data = die_id;

	return status;
}

/*
    P3T1755DPZ (Temperature Sensor) Driver
    Datasheet: https://www.nxp.com/docs/en/data-sheet/P3T1755.pdf
*/

#include "p3t1755.h"

void p3t1755_init(p3t1755_t *p3t, WritePtr write, ReadPtr read, uint16_t dev_addr) 
{
    p3t->write = write;
    p3t->read = read;
    p3t->dev_addr = dev_addr;
}

int p3t1755_write_reg(p3t1755_t *p3t, uint16_t reg, uint16_t *data) 
{
    return p3t->write(p3t->dev_addr, reg, data);
}

int p3t1755_read_reg(p3t1755_t *p3t, uint16_t reg, uint16_t *data) 
{
    return p3t->read(p3t->dev_addr, reg, data);
}

int p3t1755_read_temperature(p3t1755_t *p3t, float *temp_c)
{
    uint16_t temp_reg;

	int status = p3t1755_read_reg(p3t, p3t1755_TEMPERATURE, &temp_reg);
	if (status != 0) {
		return status;
	}

	*temp_c = p3t1755_RAW_TO_CELSIUS(temp_reg);
    return status;
}

int p3t1755_configure(p3t1755_t *p3t, uint8_t shutdown, uint8_t thermostat, 
    uint8_t polarity, uint8_t fault_queue, uint8_t conversion_time) 
{
    uint16_t config = 0;

    if (shutdown) {
        config |= p3t1755_SHUTDOWN_MODE_MASK;
    }
    if (thermostat) {
        config |= p3t1755_THERMOSTAT_MODE_MASK;
    }
    if (polarity) {
        config |= p3t1755_POLARITY_MASK;
    }
    config |= (fault_queue & p3t1755_FAULT_QUEUE_MASK);
    config |= (conversion_time & p3t1755_CONVERSION_TIME_MASK);

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_set_shutdown_mode(p3t1755_t *p3t, uint8_t enable)
{
    uint16_t config;

    int status = p3t1755_read_reg(p3t, p3t1755_CONFIGURATION, &config);
    if (status != 0) {
		return status;
	}

    if (enable != 0) {
        config |= p3t1755_SHUTDOWN_MODE_MASK;
    } else {
        config &= ~p3t1755_SHUTDOWN_MODE_MASK;
    }

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_set_thermostat_mode(p3t1755_t *p3t, uint8_t enable) 
{
    uint16_t config;

    int status = p3t1755_read_reg(p3t, p3t1755_CONFIGURATION, &config);
    if (status != 0) {
		return status;
	}

    if (enable != 0) {
        config |= p3t1755_THERMOSTAT_MODE_MASK;
    } else {
        config &= ~p3t1755_THERMOSTAT_MODE_MASK;
    }

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_set_one_shot_mode(p3t1755_t *p3t, uint8_t enable) 
{
    uint16_t config;

    int status = p3t1755_read_reg(p3t, p3t1755_CONFIGURATION, &config);
    if (status != 0) {
		return status;
	}

    if (enable != 0) {
        config |= p3t1755_ONE_SHOT_MASK;
    } else {
        config &= ~p3t1755_ONE_SHOT_MASK;
    }

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_set_polarity(p3t1755_t *p3t, uint8_t setting) 
{
    uint16_t config;

    int status = p3t1755_read_reg(p3t, p3t1755_CONFIGURATION, &config);
    if (status != 0) {
		return status;
	}

    if (setting != 0) {
        config |= p3t1755_POLARITY_MASK;
    } else {
        config &= ~p3t1755_POLARITY_MASK;
    }

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_set_fault_queue(p3t1755_t *p3t, uint8_t data) 
{
    uint16_t config;

    int status = p3t1755_read_reg(p3t, p3t1755_CONFIGURATION, &config);
    if (status != 0) {
		return status;
	}

    config &= ~p3t1755_FAULT_QUEUE_MASK;
    config |= (data & p3t1755_FAULT_QUEUE_MASK);

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_set_conversion_time(p3t1755_t *p3t, uint8_t data) 
{
    uint16_t config;

    int status = p3t1755_read_reg(p3t, p3t1755_CONFIGURATION, &config);
    if (status != 0) {
		return status;
	}

    config &= ~p3t1755_CONVERSION_TIME_MASK;
    config |= (data & p3t1755_CONVERSION_TIME_MASK);

    return p3t1755_write_reg(p3t, p3t1755_CONFIGURATION, &config);
}

int p3t1755_read_high_temp(p3t1755_t *p3t, float *temp_c) 
{
    uint16_t high_temp_reg;

    int status = p3t1755_read_reg(p3t, p3t1755_T_HIGH, &high_temp_reg);
    if (status != 0) {
		return status;
	}

    *temp_c = p3t1755_RAW_TO_CELSIUS(high_temp_reg);
    return status;
}

int p3t1755_read_low_temp(p3t1755_t *p3t, float *temp_c) 
{
    uint16_t low_temp_reg;

    int status = p3t1755_read_reg(p3t, p3t1755_T_LOW, &low_temp_reg);
    if (status != 0) {
		return status;
	}

    *temp_c = p3t1755_RAW_TO_CELSIUS(low_temp_reg);
    return status;
}

int p3t1755_set_high_temp(p3t1755_t *p3t, float temp_c) 
{
    uint16_t raw_temp = p3t1755_CELSIUS_TO_RAW(temp_c);

    return p3t1755_write_reg(p3t, p3t1755_T_HIGH, &raw_temp);
}

int p3t1755_set_low_temp(p3t1755_t *p3t, float temp_c)
{
    uint16_t raw_temp = p3t1755_CELSIUS_TO_RAW(temp_c);

    return p3t1755_write_reg(p3t, p3t1755_T_LOW, &raw_temp);
}
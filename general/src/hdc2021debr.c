#include "hdc2021debr.h"
#include <stdbool.h>
#include <stdio.h>

static int hdc2021_write_reg(hdc2021debr_t *hdc2021, uint8_t reg, uint8_t data) 
{
	uint8_t buffer[2] = {reg, data};
	return hdc2021->write_reg(buffer, hdc2021->dev_address, sizeof(buffer));
}

static int hdc2021_read_reg(hdc2021debr_t *hdc2021, uint8_t reg, uint8_t *data,
			  uint8_t length) 
{
    return hdc2021->read_reg(data, reg, hdc2021->dev_address, length);
}

int hdc2021_init(hdc2021debr_t *hdc2021, Write_ptr write_reg, Read_ptr read_reg,
		    uint8_t dev_address)
{
	hdc2021->write_reg = write_reg;
	hdc2021->read_reg = read_reg;
	hdc2021->dev_address = dev_address << 1u;

	return 0;
}

int hdc2021_toggle_heater(hdc2021debr_t *hdc2021, bool enable)
{
	uint8_t device_config;

	if (hdc2021_read_reg(hdc2021, REG_DEVICE_CONFIG, &device_config,
			  sizeof(device_config))) 
	{
		return 1;
	};

	// the 3rd bit toggles the heater
	if (enable) {
		device_config |= (1 << 3);
	} else {
		device_config &= ~(1 << 3);
	};

	if (hdc2021_write_reg(hdc2021, REG_DEVICE_CONFIG, device_config)) 
	{
		return 1;
	};

	return 0;

}
/**
 * @brief triggers oneshot measurement
 *
 * @param hdc2021 - hdc2021debr driver
 * @return int - Status code
 */
int hdc2021_trigger_oneshot(hdc2021debr_t *hdc2021)
{
    uint8_t config;

    if (hdc2021_read_reg(hdc2021, REG_MEASURE_CONFIG, &config, 1)) {
        return 1;
    }

    config |= 0x01;

    if (hdc2021_write_reg(hdc2021, REG_MEASURE_CONFIG, config)) {
        return 1;
    }

    return 0;
}

int hdc2021_get_temp_humid(hdc2021debr_t *hdc2021) 
{
	uint8_t buffer[4];

	if (hdc2021_trigger_oneshot(hdc2021)) {
		return 1;
	}


    if (hdc2021_read_reg(hdc2021, REG_TEMP_LOW, buffer, 4)) 
    {
        return 1;
    };

    uint16_t temp_bytes = (buffer[1] << 8) | buffer[0];
    uint16_t humid_bytes = (buffer[3] << 8) | buffer[2];

    float tempVal = (((float)temp_bytes/65536.0f) * 165.0f) - 40.0f;
    hdc2021->temp = tempVal;

    float humidVal = ((float)humid_bytes/65536.0f) * 100.0f;
    hdc2021->humidity = humidVal;

    return 0;
}
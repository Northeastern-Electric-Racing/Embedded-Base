#include "sht30.h"
#include <stdbool.h>
#include <stdio.h>
//ner flash --ftdi for msb

union Data {
	struct __attribute__((packed)) {
		uint16_t temp; // The packed attribute does not correctly arrange the bytes
		uint8_t temp_crc;
		uint16_t humidity; // The packed attribute does not correctly arrange the bytes
		uint8_t humidity_crc;
	} raw_data;
	uint8_t databuf[6];
} data;

static int sht30_write_reg(sht30_t *sht30, sht3x_command_t command)
{
	uint8_t command_buffer[2] = { (uint8_t)(command & 0xff00) >> 8,
				      (uint8_t)(command & 0xff) };

	return sht30->write_reg(command_buffer, sht30->dev_address, 0,
				sizeof(command_buffer));
}

static int sht30_read_reg(sht30_t *sht30, uint8_t *data)
{
	return sht30->read_reg(data, sht30->dev_address, 0, sizeof(data));
}

/**
 * @brief Calculates the CRC by using the polynomial x^8 + x^5 + x^4 + 1
 * @param data: the data to use to calculate the CRC
 */
static uint8_t calculate_crc(const uint8_t *data, size_t length)
{
	uint8_t crc = 0xff;
	for (size_t i = 0; i < length; i++) {
		crc ^= data[i];
		for (size_t j = 0; j < 8; j++) {
			if ((crc & 0x80) != 0) {
				crc = (uint8_t)((uint8_t)(crc << 1) ^ 0x91);
			} else {
				crc <<= 1;
			}
		}
	}
	return crc;
}

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb)
{
	return (uint16_t)((uint16_t)msb << 8) | lsb;
}

int sht30_init(sht30_t *sht30, Write_ptr write_reg, Read_ptr read_reg,
	       uint8_t dev_address)
{
	sht30->write_reg = write_reg;
	sht30->read_reg = read_reg;
	sht30->dev_address = dev_address << 1u;

	uint8_t status_reg_and_checksum[3];
	if (sht30->read_reg(status_reg_and_checksum, sht30->dev_address,
			    (uint8_t)SHT3X_COMMAND_READ_STATUS,
			    sizeof(status_reg_and_checksum)) != 0) {
		return -1;
	}

	uint8_t calculated_crc = calculate_crc(status_reg_and_checksum, 2);

	if (calculated_crc != status_reg_and_checksum[2]) {
		return -1;
	}

	return 0;
}

int sht30_toggle_heater(sht30_t *sht30, bool enable)
{
	if (enable) {
		return sht30_write_reg(sht30, SHT3X_COMMAND_HEATER_ENABLE);
	} else {
		return sht30_write_reg(sht30, SHT3X_COMMAND_HEATER_DISABLE);
	}
}

int sht30_get_temp_humid(sht30_t *sht30)
{
	uint8_t temp, humidity;

	sht30_write_reg(sht30, (SHT30_START_CMD_WCS));

	uint8_t *test;

	if (sht30_read_reg(sht30, test) != 0) {
		return -1;
	}

	temp = uint8_to_uint16(data.databuf[0], data.databuf[1]);

	if (data.raw_data.temp_crc != calculate_crc(data.databuf, 2)) {
		return -1;
	}

	float tempVal = -45.0f + 175.0f * (((float)temp) / 65535.0f);

	sht30->temp = tempVal;

	humidity = uint8_to_uint16(data.databuf[3], data.databuf[4]);
	if (data.raw_data.humidity_crc != calculate_crc(data.databuf + 3, 2)) {
		return -1;
	}

	float humVal = (100.0f * ((float)humidity / 65535.0f));

	sht30->humidity = humVal;

	return 0;
};
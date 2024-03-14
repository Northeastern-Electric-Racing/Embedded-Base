#include "sht30.h"
#include <stdio.h>
#include <stdbool.h>

#define SWITCHBYTES(x)             (((x<<8) | (x>>8)) & 0xFFFF)      //Switches the first and second byte (This is required for all command values)

typedef enum
{
	SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH = 0x2c06,
	SHT3X_COMMAND_CLEAR_STATUS = 0x3041,
	SHT3X_COMMAND_SOFT_RESET = 0x30A2,
	SHT3X_COMMAND_HEATER_ENABLE = 0x306d,
	SHT3X_COMMAND_HEATER_DISABLE = 0x3066,
	SHT3X_COMMAND_READ_STATUS = 0xf32d,
	SHT3X_COMMAND_FETCH_DATA = 0xe000,
	SHT3X_COMMAND_MEASURE_HIGHREP_10HZ = 0x2737,
	SHT3X_COMMAND_MEASURE_LOWREP_10HZ = 0x272a
} sht3x_command_t;

static HAL_StatusTypeDef sht30_read_reg(sht30_t *sht30, uint8_t *data, uint16_t reg, uint8_t size)
{
    return HAL_I2C_Mem_Read(sht30->i2c_handle, SHT30_I2C_ADDR, reg, I2C_MEMADD_SIZE_16BIT, data, size, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef sht30_write_reg(sht30_t *sht30, uint16_t command)
{
    uint8_t command_buffer[2] = {(command & 0xff00u) >> 8u, command & 0xffu};

	if (HAL_I2C_Master_Transmit(sht30->i2c_handle, SHT30_I2C_ADDR, command_buffer, sizeof(command_buffer),
	                            30) != HAL_OK) {
		return false;
	}

	return true;
    // return HAL_I2C_Mem_Write(sht30->i2c_handle, SHT30_I2C_ADDR, reg, I2C_MEMADD_SIZE_16BIT, data, size, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef sht30_handle_status_reg(sht30_t *sht30)
{
    HAL_StatusTypeDef status;
    uint8_t data[3];
    uint16_t register_status;

    status = sht30_write_reg(sht30, (SHT30_READSTATUS));
    if (status != HAL_OK)
        return status;

    status = sht30_read_reg(sht30, data, (SHT30_READSTATUS), 3);
    if (status != HAL_OK)
        return status;

    register_status = data[0];
    register_status <<= 8;
    register_status |= data[1];
    
    sht30->status_reg = register_status;
    return HAL_OK;
}

/**
 * @brief Calculates the CRC by using the polynomial x^8 + x^5 + x^4 + 1
 * @param data: the data to use to calculate the CRC
*/
static uint8_t calculate_crc(const uint8_t *data, size_t length) {
    uint8_t crc = 0xff;
	for (size_t i = 0; i < length; i++) {
		crc ^= data[i];
		for (size_t j = 0; j < 8; j++) {
			if ((crc & 0x80u) != 0) {
				crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
			} else {
				crc <<= 1u;
			}
		}
	}
	return crc;
}

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb)
{
	return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

HAL_StatusTypeDef sht30_reset(sht30_t *sht30)
{
    HAL_StatusTypeDef status = HAL_OK;
    status = sht30_write_reg(sht30, SHT30_SOFTRESET);
    if (status != HAL_OK) {
        return status;
    }
    return status;
}

HAL_StatusTypeDef sht30_init(sht30_t *sht30)
{
    HAL_StatusTypeDef status = HAL_OK;

    uint8_t status_reg_and_checksum[3];
	if (HAL_I2C_Mem_Read(sht30->i2c_handle, SHT30_I2C_ADDR, SHT3X_COMMAND_READ_STATUS, 2, (uint8_t*)&status_reg_and_checksum,
					  sizeof(status_reg_and_checksum), 30) != HAL_OK) {
		return false;
	}

	uint8_t calculated_crc = calculate_crc(status_reg_and_checksum, 2);

	if (calculated_crc != status_reg_and_checksum[2]) {
		return false;
	}

    return status;
}

HAL_StatusTypeDef sht30_is_heater_enabled(sht30_t *sht30)
{
    HAL_StatusTypeDef status;
    status = sht30_handle_status_reg(sht30);
    if (status != HAL_OK)
        return status;

    sht30->is_heater_enabled = (bool)((sht30->status_reg >> SHT30_REG_HEATER_BIT) & 0x01);

    return HAL_OK;
}

HAL_StatusTypeDef sht30_toggle_heater(sht30_t *sht30)
{
    uint16_t cmdVal = sht30->is_heater_enabled ? (SHT30_HEATEREN) : (SHT30_HEATERDIS);
    HAL_StatusTypeDef status;

    status = sht30_write_reg(sht30, cmdVal);
    if (status != HAL_OK)
        return status;

    return sht30_is_heater_enabled(sht30);
}

HAL_StatusTypeDef sht30_get_temp_humid(sht30_t *sht30)
{
    HAL_StatusTypeDef status;

    union
    {
        struct __attribute__((packed))
        {
            uint16_t temp;
            uint8_t temp_crc;
            uint16_t humidity;
            uint8_t humidity_crc;
        } raw_data;
        uint8_t databuf[6];
    } data;

    uint16_t temp, humidity;

    sht30_write_reg(sht30, (SHT30_START_CMD_WCS));

    HAL_Delay(1);

    status = HAL_I2C_Master_Receive(sht30->i2c_handle, SHT30_I2C_ADDR, data.databuf, sizeof(data.databuf), 30);
    if (status != HAL_OK) {
        printf("failed to read temperature, %d", status);
		return false;
	}


    temp = uint8_to_uint16(data.databuf[0], data.databuf[1]);

    if (data.raw_data.temp_crc != calculate_crc(data.databuf, 2)) {
        printf("Raw temp data failed CRC\r\n");
        return HAL_ERROR;
    }

    float val = -45.0f + 175.0f * (float)temp / 65535.0f;

    printf("temp raw: %f", val);

    sht30->temp = (uint16_t)val;

    printf("temp: %d \n", sht30->temp);

    humidity = uint8_to_uint16(data.databuf[3], data.databuf[4]);
    if (data.raw_data.humidity_crc != calculate_crc(data.databuf + 3, 2)) {
        printf("Could not calculate crc for humidity\r\n");
        return HAL_ERROR;
    }

    humidity = (uint16_t)(100.0f * (float)humidity / 65535.0f);
    sht30->humidity = humidity;

    printf("Humidity: %d \n", sht30->humidity);

    return HAL_OK;
}
#include "sht30.h"

static int sht30_write_reg(const sht30_t *sht30, uint16_t command) {
  uint8_t command_buffer[2] = {(uint8_t)((command & 0xff00u) >> 8u), (uint8_t)(command & 0xffu)};
  return sht30->i2c_transmit(SHT30_I2C_ADDR, command_buffer, sizeof(command_buffer), 30u);
}

/**
 * @brief Calculates the CRC by using the polynomial x^8 + x^5 + x^4 + 1
 * @param data: the data to use to calculate the CRC
 */
static uint8_t calculate_crc(const uint8_t *data, size_t length) {
  uint8_t crc = 0xffu;
  for (size_t i = 0u; i < length; i++) {
    crc ^= data[i];
    for (uint8_t j = 0u; j < 8u; j++) {
      if ((crc & 0x80u) != 0u) {
        crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
      } else {
        crc = (uint8_t)(crc << 1u);
      }
    }
  }
  return crc;
}

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb) {
  return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

int sht30_init(sht30_t *sht30) {
  uint8_t status_reg_and_checksum[3];
  int status = sht30->i2c_mem_read(SHT30_I2C_ADDR, SHT3X_COMMAND_READ_STATUS, 2u,
                                   status_reg_and_checksum, sizeof(status_reg_and_checksum), 30u);
  if (status != 0) {
    return status;
  }

  uint8_t calculated_crc = calculate_crc(status_reg_and_checksum, 2u);
  if (calculated_crc != status_reg_and_checksum[2]) {
    return -1;
  }

  return 0;
}

int sht30_toggle_heater(sht30_t *sht30, bool enable) {
    if (enable) {
        return sht30_write_reg(sht30, SHT3X_COMMAND_HEATER_ENABLE);
    } else {
        return sht30_write_reg(sht30, SHT3X_COMMAND_HEATER_DISABLE);
    }
}

int sht30_get_temp_humid(sht30_t *sht30) {
  union {
    struct __attribute__((packed)) {
      uint16_t temp;
      uint8_t temp_crc;
      uint16_t humidity;
      uint8_t humidity_crc;
    } raw_data;
    uint8_t databuf[6];
  } data;

  int status = sht30_write_reg(sht30, SHT30_START_CMD_WCS);
  if (status != 0) {
    return status;
  }

  status = sht30->i2c_receive(SHT30_I2C_ADDR, data.databuf, sizeof(data.databuf), 30u);
  if (status != 0) {
    return status;
  }

  uint16_t temp = uint8_to_uint16(data.databuf[0], data.databuf[1]);
  if (data.raw_data.temp_crc != calculate_crc(data.databuf, 2u)) {
    return -1;
  }

  sht30->temp = (uint16_t)(temp_val / 1000u);

  uint16_t humidity = uint8_to_uint16(data.databuf[3], data.databuf[4]);
  if (data.raw_data.humidity_crc != calculate_crc(data.databuf + 3, 2u)) {
    return -1;
  }

  humidity = (uint16_t)(100.0f * (float)humidity / 65535.0f);
  sht30->humidity = humidity;

  return 0;
}

#include "max7314.h"

// mode: 0 for output, 1 for input
HAL_StatusTypeDef MAX7314_set_pin(uint8_t pin, uint8_t mode) {
  uint8_t confData[8];
  if (pin < 7) {
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x06, 8, &confData, 8, 2000);
    confData[pin] = mode;
    HAL_I2C_Mem_Write(i2c_handle, DEV_ADDR, 0x06, 8, &confData, 8, 2000);
  } else {
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x07, 8, &confData, 8, 2000);
    confData[pin - 8] = mode;
    HAL_I2C_Mem_Write(i2c_handle, DEV_ADDR, 0x07, 8, &confData, 8, 2000);
  }
}

// Paremeter reg: 0 if all pins are on 0th register, 1 if all pins are on 1th reg,
// 2 if pins are on both. If you enter 2, it is assumed that data is an array with 16 elements.
// reg = 0 for pins 0-7, = 1 for pins 8-15, 2 for pins 0-15
// NOTE: All pins are inputs on startup
HAL_StatusTypeDef MAX7314_set_pins(uint8_t *pinConfigs, uint8_t reg) {
  if (reg < 2) {
    HAL_I2C_Mem_Write(i2c_handle, DEV_ADDR, reg, 8, pinConfigs, 8, 2000);
  } else {
    HAL_I2C_Mem_Write(i2c_handle, DEV_ADDR, 0x06, 8, pinConfigs, 8, 2000);
    HAL_I2C_Mem_Write(i2c_handle, DEV_ADDR, 0x07, 8, pinConfigs + 8, 8, 2000);
  }
}

HAL_StatusTypeDef MAX7314_read_pin(uint8_t pin, uint8_t *data) {
  uint8_t pinData[8];
  uint8_t memAdd;
  if (pin < 8) {
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x00, 8, &pinData, 8, 2000);
    *data = pinData[pin];
  } else {
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x01, 8, &pinData, 8, 2000);
    *data = pinData[pin - 8];
  }
}

// Paremeter reg: 0 if all pins are on 0th register, 1 if all pins are on 1th reg,
// 2 if pins are on both. If you enter 2, it is assumed that data is an array with 16 elements.
// reg = 0 for pins 0-7, = 1 for pins 8-15, 2 for pins 0-15
HAL_StatusTypeDef MAX7314_read_pins(uint8_t reg, uint8_t *data) {
  if (reg < 2) {
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, reg, 8, &data, 8, 2000);
  } else {
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x00, 8, data, 8, 2000);
    HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x01, 8, data + 8, 8, 2000);
  }
}

// state: off = 0, on = 1
HAL_StatusTypeDef MAX7314_set_pin_output(uint16_t pin, uint8_t state) {
  uint8_t pinData[8];
  HAL_I2C_Mem_Read(i2c_handle, DEV_ADDR, 0x10 + (pin % 2), 8, &pinData, 8, 2000);
  if (pin % 2 == 0) {
    pinData[0] = state;
    pinData[1] = state;
    pinData[2] = state;
    pinData[3] = state;
  } else {
    pinData[4] = state;
    pinData[5] = state;
    pinData[6] = state;
    pinData[7] = state;
  }
  HAL_I2C_Mem_Write(i2c_handle, DEV_ADDR, 0x10 + (pin % 2), 8, &pinData, 8, 2000);
}   
#include "max7314.h"

#define MAX7314_INPUT_PORTS_0_TO_7  0x00
#define MAX7314_INPUT_PORTS_8_TO_15 0x01
#define MAX7314_PORT_CONFIG_0_TO_7  0x06
#define MAX7314_PORT_CONFIG_8_TO_15 0x07
#define MAX7314_CONFIG_REG          0x0F
#define FIRST_OUTPUT_REGISTER       0x10

#define REG_SIZE 1 //byte
#define TIMEOUT 2000

HAL_StatusTypeDef read_reg(max7314_t *max, uint16_t address, uint8_t *data) {
    return HAL_I2C_Mem_Read(max->i2c_handle, max->dev_addr, address, I2C_MEMADD_SIZE_8BIT, data, REG_SIZE, TIMEOUT);
}

HAL_StatusTypeDef write_reg(max7314_t *max, uint16_t address, uint8_t *data) {
    return HAL_I2C_Mem_Write(max->i2c_handle, max->dev_addr, address, I2C_MEMADD_SIZE_8BIT, data, REG_SIZE, TIMEOUT);
}

HAL_StatusTypeDef max7314_init(max7314_t *max, I2C_HandleTypeDef *i2c_handle) {
    max->i2c_handle = i2c_handle;
    max->dev_addr = max->dev_addr << 1u ;  /* shifted one to the left cuz STM says so */

    uint8_t config_data = 0b01001100;
    HAL_StatusTypeDef status = max7314_write_config(max, &config_data);
    if (status != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef max7314_write_config(max7314_t *max, uint8_t *config) {
    return write_reg(max, MAX7314_CONFIG_REG, config);
}

HAL_StatusTypeDef max7314_set_pin_mode(max7314_t *max, uint8_t pin, max7314_pin_modes_t mode)
{
    uint8_t conf_data;
    HAL_StatusTypeDef status;

    if (pin < 7) {
        /* Read current port configuration */
        status = read_reg(max, MAX7314_PORT_CONFIG_0_TO_7, &conf_data);
        if (status != HAL_OK) 
            return status;

        /* Change port configuration of desired pin. Bit manip sets one bit to mode. */
        conf_data = (conf_data & ~(1u << pin)) | (mode << pin);
        status = write_reg(max, MAX7314_PORT_CONFIG_0_TO_7, &conf_data);
        if (status != HAL_OK) 
            return status;

    } else {
        /* Same as above but for different register */
        status = read_reg(max, MAX7314_PORT_CONFIG_8_TO_15, &conf_data);
        if (status != HAL_OK) 
            return status;
        
        conf_data = (conf_data & ~(1u << (pin - REG_SIZE))) | (mode << (pin - REG_SIZE));
        status = write_reg(max, MAX7314_PORT_CONFIG_8_TO_15, &conf_data);
        if (status != HAL_OK) 
            return status;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef max7314_set_pin_modes(max7314_t *max, max7314_pin_regs_t reg, uint8_t *pin_configs)
{
    return write_reg(max, MAX7314_PORT_CONFIG_0_TO_7 + reg, pin_configs);
}

HAL_StatusTypeDef max7314_read_pin(max7314_t *max, uint8_t pin, bool *state) 
{
    uint8_t pin_data;
    HAL_StatusTypeDef status;

    if (pin < 8) {
        /* similar to set_pin_mode */
        status = read_reg(max, MAX7314_INPUT_PORTS_0_TO_7, &pin_data);
        if (status != HAL_OK) 
            return status;

        *state = pin_data & (1u << pin);
    } else {
        status = read_reg(max, MAX7314_INPUT_PORTS_8_TO_15, &pin_data);
        if (status != HAL_OK) 
            return status;

        *state = pin_data & (1u << (pin - REG_SIZE));
    }

    return HAL_OK;
}

HAL_StatusTypeDef max7314_set_pin_state(max7314_t *max, uint8_t pin, bool state) 
{
    uint8_t pin_data;
    HAL_StatusTypeDef status;
    
    /* 2 pins, each 4 bits, per register */
    status = read_reg(max, FIRST_OUTPUT_REGISTER + (pin % 2), &pin_data);
    if (status != HAL_OK) 
        return status;

    /* Bit manipulation changes only the bits we want to. */
    if (state) {
        pin_data = (pin_data & ~(0b1111 << (pin % 2))) | (0b1111 << (pin % 2));
    } else {
        pin_data = (pin_data & ~(0b0000 << (pin % 2))) | (0b0000 << (pin % 2));
    }

    /* Write to register containing the desired pin */
    status = write_reg(max, FIRST_OUTPUT_REGISTER + (pin % 2), &pin_data);
    
    return status;
}   

HAL_StatusTypeDef max7314_read_pin_state(max7314_t *max, uint8_t pin, bool* state) 
{
    uint8_t pin_data;
    HAL_StatusTypeDef status;
    
    if (pin < 8) {
        status = read_reg(max, 0x00, &pin_data);
        pin_data &= 1u << pin;
    } else {
        status = read_reg(max, 0x01, &pin_data);
        pin_data &= 1u << (pin - REG_SIZE);
    }

    if (status != HAL_OK)
        return status;
    
    *state = pin_data > 0;

    return status;
}
#include "max7314.h"

#define MAX7314_INPUT_PORTS_0_TO_7  0x00
#define MAX7314_INPUT_PORTS_8_TO_15 0x01
#define MAX7314_PORT_CONFIG_0_TO_7  0x06
#define MAX7314_PORT_CONFIG_8_TO_15 0x07
#define MAX7314_CONFIG_REG          0x0F
#define FIRST_OUTPUT_REGISTER       0x10

#define REG_SIZE 1
#define TIMEOUT 2000


HAL_StatusTypeDef read_reg(max7314_t *max, uint16_t address, uint8_t *data) {
    return HAL_I2C_Mem_Read(max->i2c_handle, max->dev_addr, address, I2C_MEMADD_SIZE_8BIT, data, REG_SIZE, TIMEOUT);
}

HAL_StatusTypeDef write_reg(max7314_t *max, uint16_t address, uint8_t *data) {
    return HAL_I2C_Mem_Write(max->i2c_handle, max->dev_addr, address, I2C_MEMADD_SIZE_8BIT, data, REG_SIZE, TIMEOUT);
}

HAL_StatusTypeDef max7314_init(max7314_t *max, I2C_HandleTypeDef *i2c_handle) {
    max->i2c_handle = i2c_handle;
    max->dev_addr = max->dev_addr << 1u;  /* shifted one to the left cuz STM says so */

    /* Check to see if i2c is working */
    uint8_t data[1];
    HAL_StatusTypeDef status = read_reg(max, MAX7314_CONFIG_REG, data);
    if (status != HAL_OK)
        return status;

    if (data != 0b00001100) /* configuration register startup state */
        return HAL_ERROR;

    return status;
}

HAL_StatusTypeDef max7314_write_config(max7314_t *max, uint8_t *config) {
    return write_reg(max, MAX7314_CONFIG_REG, config);
}

HAL_StatusTypeDef max7314_set_pin_mode(max7314_t *max, uint8_t pin, max7314_pin_modes_t mode)
{
    uint8_t conf_data[1];
    HAL_StatusTypeDef status;

    if (pin < 8) {
        /* Read current port configuration */
        status = read_reg(max, MAX7314_PORT_CONFIG_0_TO_7, conf_data);
        if (status != HAL_OK) 
            return status;

        /* Change port configuration of desired pin */
        uint8_t mask = 1 << pin; 
        conf_data[0] = ((conf_data[0] & ~mask) | (mode << pin)); // set bit to specific value
        status = write_reg(max, MAX7314_PORT_CONFIG_0_TO_7, conf_data);
        if (status != HAL_OK) 
            return status;

    } else {
        /* Same as above but for different register */
        status = read_reg(max, MAX7314_PORT_CONFIG_8_TO_15, conf_data);
        if (status != HAL_OK) 
            return status;
        
        uint8_t mask = 1 << pin; 
        conf_data[0] = ((conf_data[0] & ~mask) | (mode << pin)); // set
        status = write_reg(max, MAX7314_PORT_CONFIG_8_TO_15, conf_data);
        if (status != HAL_OK) 
            return status;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef max7314_set_pin_modes(max7314_t *max, max7314_pin_regs_t reg, uint8_t *pin_configs)
{
    HAL_StatusTypeDef status;

    if (reg < 2) {
        /* Set pin modes of one register */
        status = write_reg(max, MAX7314_PORT_CONFIG_0_TO_7 + reg, pin_configs);
        if (status != HAL_OK) 
            return status;

    } else {
        /* Set pin modes of both registers. pin_configs should have 16 elements in this case */
        status = write_reg(max, MAX7314_PORT_CONFIG_0_TO_7, pin_configs);
        if (status != HAL_OK) 
            return status;
    }
   
    return HAL_OK;
}

HAL_StatusTypeDef max7314_set_pin_state(max7314_t *max, uint16_t pin, bool state) 
{
    uint8_t pin_data[1];
    HAL_StatusTypeDef status;
    
    /* 2 pins, each 4 bits, per register */
    status = read_reg(max, FIRST_OUTPUT_REGISTER + (pin % 2), pin_data);
    if (status != HAL_OK) 
        return status;

    if (pin % 2 == 0) {
        pin_data[0] = state;
        pin_data[1] = state;
        pin_data[2] = state;
        pin_data[3] = state;
    } else {
        pin_data[4] = state;
        pin_data[5] = state;
        pin_data[6] = state;
        pin_data[7] = state;
    }
    /* Write to register containing the desired pin */
    status = write_reg(max, FIRST_OUTPUT_REGISTER + (pin % 2), pin_data);
    if (status != HAL_OK) 
        return status;
    
    return HAL_OK;
}   

HAL_StatusTypeDef max7314_read_pin_state(max7314_t *max, uint16_t pin, bool *state) 
{
    uint8_t pin_data[1];
    HAL_StatusTypeDef status;
    
    status = read_reg(max, FIRST_OUTPUT_REGISTER + (pin % 2), pin_data);
    if (status != HAL_OK) 
        return status;
    
    if (pin % 2 == 0)
        pin_data[0] = pin_data[0] & ~(0b10000000 | 0b01000000 | 0b00100000 | 0b00010000)
    else
        pin_data[0] = pin_data[0] & ~0b00001111

    // With the other bits cleared, if the number is greater than zero, then the pin is on
    *state = pin_data[0] > 0;

    return HAL_OK;
}
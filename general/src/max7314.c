#include "max7314.h"

#define MAX7314_INPUT_PORTS_0_TO_7  0x00
#define MAX7314_INPUT_PORTS_8_TO_15 0x01
#define MAX7314_PHASE_0_OUTPUTS_0_7     0x02
#define MAX7314_PHASE_0_OUTPUTS_8_15    0x03
#define MAX7314_PORT_CONFIG_0_TO_7  0x06
#define MAX7314_PORT_CONFIG_8_TO_15 0x07
#define MAX7314_MASTER_INTENSITY    0x0E
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

HAL_StatusTypeDef write_word(max7314_t *max, uint16_t address, uint8_t *data) {
    return HAL_I2C_Mem_Write(max->i2c_handle, max->dev_addr, address, I2C_MEMADD_SIZE_8BIT, data, REG_SIZE * 2, TIMEOUT);
}

void max7314_init(max7314_t *max, I2C_HandleTypeDef *i2c_handle) {
    max->i2c_handle = i2c_handle;
    max->dev_addr = max->dev_addr << 1u ;  /* shifted one to the left cuz STM says so */
}


HAL_StatusTypeDef max7314_set_global_intensity(max7314_t *max, uint8_t level) {

    HAL_StatusTypeDef status;
    uint8_t master_intensity;

    if (level == 0) {
        master_intensity = 0x00;
    } else {
        master_intensity = 0x0F;
    }

    status = write_reg(max, MAX7314_MASTER_INTENSITY, &master_intensity);
    if (status != HAL_OK) {
        return status;
    }
}

    // PHASE 0 OUTPUT BIT 1: PIN HIGH NOT PULLED DOWN
    // PHASE 0 OUTPUT BIT 0: PIN LOW PULLED DOWN

    // uint8_t do_nothing = 0b11111111;
    // write_reg(max, MAX7314_PHASE_0_OUTPUTS_0_7, &do_nothing);

HAL_StatusTypeDef max7314_write_config(max7314_t *max, uint8_t *config) {
    return write_reg(max, MAX7314_CONFIG_REG, config);
}

HAL_StatusTypeDef max7314_set_pin_mode(max7314_t *max, uint8_t pin, uint8_t mode)
{
    uint8_t conf_data;
    HAL_StatusTypeDef status;

    if (pin < 7) {
        /* Read current port configuration */
        status = read_reg(max, MAX7314_PHASE_0_OUTPUTS_0_7, &conf_data);
        if (status != HAL_OK) 
            return status;

        /* Change port configuration of desired pin. Bit manip sets one bit to mode. */
        conf_data = (conf_data & ~(1u << pin)) | (mode << pin);
        status = write_reg(max, MAX7314_PHASE_0_OUTPUTS_0_7, &conf_data);
        if (status != HAL_OK) 
            return status;

    } else {
        /* Same as above but for different register */
        status = read_reg(max, MAX7314_PHASE_0_OUTPUTS_8_15, &conf_data);
        if (status != HAL_OK) 
            return status;
        
        conf_data = (conf_data & ~(1u << (pin - REG_SIZE))) | (mode << (pin - REG_SIZE));
        status = write_reg(max, MAX7314_PHASE_0_OUTPUTS_8_15, &conf_data);
        if (status != HAL_OK) 
            return status;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef max7314_set_pin_modes(max7314_t *max, uint8_t *pin_configs) {
    return write_word(max, MAX7314_PORT_CONFIG_0_TO_7, pin_configs);
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

        *state = (pin_data & (1u << pin)) > 0;
    } else {
        status = read_reg(max, MAX7314_INPUT_PORTS_8_TO_15, &pin_data);
        if (status != HAL_OK) 
            return status;

        *state = (pin_data & (1u << (pin - REG_SIZE))) > 0;
    }

    return HAL_OK;
}

HAL_StatusTypeDef max7314_set_pin_state(max7314_t *max, uint8_t pin, bool state) 
{
    uint8_t pin_data;
    HAL_StatusTypeDef status;
    uint8_t reg = MAX7314_PHASE_0_OUTPUTS_0_7;
    uint8_t pin_state;

    if (pin > 7) {
        reg += 1;
    }

    if (state) {
        pin_state = 1;
    } else {
        pin_state = 0;
    }

    status = read_reg(max, reg, &pin_data);
    if (status != HAL_OK) 
       return status;

    /* Clear bit at position, then set it to state*/
    if (pin < 8) {
        pin_data = (pin_data & ~(1u << pin)) | (state << pin);
    } else {
        pin_data = (pin_data & ~(1u << (pin - REG_SIZE))) | (state << (pin - REG_SIZE));
    }

    // /* Write to register containing the desired pin */
    status = write_reg(max, reg, &pin_data);
    
    return HAL_OK;
}   
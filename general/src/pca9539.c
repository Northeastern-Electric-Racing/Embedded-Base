#include "pca9539.h"

#define REG_SIZE_BITS 8
#define TIMEOUT HAL_MAX_DELAY

typedef enum
{
    PCA9539_INPUT = 0,
    PCA9539_OUTPUT = 2,
    PCA9539_POLARITY = 4,
    PCA9539_DIRECTION = 6,
} register_type_t;

HAL_StatusTypeDef pca_write_reg(pca9539_t *pca, uint16_t address, pca9539_port_t port, uint8_t *data)
{
    if (port == PCA_B1)
    {
        address = address + 1;
    }
    // ensure shifting left one, HAL adds the write bit
    return HAL_I2C_Mem_Write(pca->i2c_handle, pca->dev_addr << 1, address, I2C_MEMADD_SIZE_8BIT, data, sizeof(data), TIMEOUT);
}

HAL_StatusTypeDef pca_read_reg(pca9539_t *pca, uint16_t address, pca9539_port_t port, uint8_t *data)
{
    if (port == PCA_B1)
    {
        address = address + 1;
    }

    return HAL_I2C_Mem_Read(pca->i2c_handle, pca->dev_addr << 1, address, I2C_MEMADD_SIZE_8BIT, data, sizeof(data), TIMEOUT);
}

static uint8_t create_buf(pca9539_pin_mode_t config[8])
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        data |= config[i] << i;
    }
    return data;
}

static void deconstruct_buf(uint8_t *data, pca9539_pin_mode_t *config)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        config[i] = (*data >> i) & 1;
    }
}

void pca9539_init(pca9539_t *pca, I2C_HandleTypeDef *i2c_handle, pca9539_addr_t *dev_addr)
{
    pca->i2c_handle = i2c_handle;
    pca->dev_addr = pca->dev_addr << 1u; /* shifted one to the left cuz STM says so */
}

HAL_StatusTypeDef read_pins(pca9539_t *pca, pca9539_reg_type_t reg_type, pca9539_port_t port, pca9539_pin_mode_t config[8])
{

    uint8_t data = create_buf(config);
    HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, port, &data);
    if (status)
    {
        return status;
    }

    deconstruct_buf(&data, config);

    return status;
}

HAL_StatusTypeDef read_pin(pca9539_t *pca, pca9539_reg_type_t reg_type, pca9539_port_t port, pca9539_pins_t pin, pca9539_pin_mode_t *config)
{
    uint8_t data;
    HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, port, &data);
    if (status)
    {
        return status;
    }

    *config = (data & (0b1 << pin)) > 0;

    return status;
}

HAL_StatusTypeDef write_pins(pca9539_t *pca, pca9539_reg_type_t reg_type, pca9539_port_t port, pca9539_pin_mode_t config[8])
{

    uint8_t buf = create_buf(config);
    return pca_write_reg(pca, reg_type, port, &buf);
}

HAL_StatusTypeDef write_pin(pca9539_t *pca, pca9539_reg_type_t reg_type, pca9539_port_t port, pca9539_pins_t pin, pca9539_pin_mode_t config)
{

    uint8_t data;

    HAL_StatusTypeDef status = pca_read_reg(pca, reg_type, port, &data);
    if (status)
    {
        return status;
    }

    uint8_t data_new = (data & ~(1u << pin)) | (config << pin);

    return pca_write_reg(pca, reg_type, port, &data_new);
}

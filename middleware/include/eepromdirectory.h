#ifndef EEPROMDIRECTORY_H
#define EEPROMDIRECTORY_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

typedef HAL_StatusTypedef (*eeprom_write)(I2C_HandleTypeDef *i2c_handle, uint16_t mem_address, uint8_t *data, uint16_t size);
typedef HAL_StatusTypedef (*eeprom_read)(I2C_HandleTypeDef *i2c_handle, uint16_t mem_address, uint8_t *data, uint16_t size);
typedef HAL_StatusTypedef (*eeprom_delete)(I2C_HandleTypeDef *i2c_handle, uint16_t mem_address, uint16_t size);

typedef struct
{
    char *id;           /*  key  */
    uint16_t size;      /* bytes */
    uint16_t address;     /* start address */
} eeprom_partition_t;

typedef struct {
    eeprom_write write_cb;
    eeprom_read read_cb;
    eeprom_delete delete_cb;
} eepromfs_t;

void eeprom_init(eepromfs_t *eeprom, eeprom_write write_cb, eeprom_read read_cb, eeprom_delete delete_cb);

/**
 * @brief returns the starting address of the passed key
 *
 * @param key
 * @return int
 */
int eeprom_get_index(char *key);

/**
 * @brief returns the key at the passed index
 *
 *
 */
char *eeprom_get_key(int index);

/**
 * @brief fills passed data pointer with data from eeprom
 *
 * @note user is responsible for passing data of correct size
 * @param key
 * @param data
 */

/* declaration of function taking in array of structures for filling in the parameters of the fault conditions*/
void eeprom_init(struct eeprom_partition partition_list[], int array_size);
void eeprom_read_data_key(char *key, void *data);
void eeprom_read_data_index(uint8_t index, void *data); 

/**
 * @brief loads eeprom with data from passed pointer
 *
 * @note user is responsible for passing data of correct size
 * @param key
 * @param data
 */
void eeprom_write_data_key(char *key, void *data);

void eeprom_write_data_index(uint8_t index, void *data);


#endif // EEPROMDIRECTORY_H
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
    uint16_t address;   /* start address */
} eeprom_partition_t;

typedef struct {
    I2C_HandleTypeDef *i2c_handle;
    eeprom_write write_cb;
    eeprom_read read_cb;
    eeprom_delete delete_cb;
    eeprom_partition_t *eeprom_data;
} eepromfs_t;

/* Initialize the callbacks for an EEPROM struct */
void eeprom_init(eepromfs_t *eeprom, eeprom_write write_cb, eeprom_read read_cb, eeprom_delete delete_cb);

/* declaration of function taking in array of structures for filling in the parameters of the fault conditions*/
void eeprom_partition(eepromfs_t eeprom, struct eeprom_partition partition_list[], int array_size);

/* Retrieve the index of a certain key */
int eeprom_get_index(eepromfs_t eeprom, char *key);

/* Retrieve the key at a certain index */
char *eeprom_get_key(eepromfs_t eeprom, int index);

/* read the data at a certain key */
void eeprom_read_data_key(eepromfs_t eeprom, char *key, void *data);

/* Read the data at a certain index */
void eeprom_read_data_index(eepromfs_t eeprom, uint8_t index, void *data);

/* Write data at a certain key */
void eeprom_write_data_key(eepromfs_t eeprom, char *key, void *data);

/* Write data at a certain index */
void eeprom_write_data_index(eepromfs_t eeprom, uint8_t index, void *data);

#endif // EEPROMDIRECTORY_H
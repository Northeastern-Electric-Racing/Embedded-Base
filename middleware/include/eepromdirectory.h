#ifndef EEPROMDIRECTORY_H
#define EEPROMDIRECTORY_H

#include <stdint.h>

struct eeprom_partition 
{
    char *id;           /*  key  */
    uint16_t size;      /* bytes */
    uint16_t address;     /* start address */
};

struct eeprom_partition eeprom_data[NUM_EEPROM_ITEMS]
{
/*  ____________KEY________________         _BYTES_   */
                       

};

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
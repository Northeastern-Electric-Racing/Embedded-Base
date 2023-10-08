#ifndef EEPROMDIRECTORY_H
#define EEPROMDIRECTORY_H

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
int eepromGetIndex(char *key);

/**
 * @brief returns the key at the passed index
 *  
 * 
 */
char *eepromGetKey(int index);



/**
 * @brief fills passed data pointer with data from eeprom
 * 
 * @note user is responsible for passing data of correct size
 * @param key
 * @param data
 */
void eepromReadData(char *key, void *data);

void eepromReadData(uint8_t index, void *data);

/**
 * @brief loads eeprom with data from passed pointer
 * 
 * @note user is responsible for passing data of correct size
 * @param key 
 * @param data 
 */
void eepromWriteData(char *key, void *data);

void eepromWriteData(uint8_t index, void *data);


#endif
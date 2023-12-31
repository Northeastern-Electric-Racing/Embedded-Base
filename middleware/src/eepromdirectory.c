#include "eepromdirectory.h"
#include "general/include/m24c32.h"

uint16_t eepromGetAddress(char *key)
{
    /* find the index of the key in the eeprom_data array */
    int i = 0;
    while (eeprom_data[i].id != NULL)
    {
        if (eeprom_data[i].id == key)
        {
            return eeprom_data[i].address;
        }

        i++;
    }
    return -1;
}

uint16_t eeprom_get_index(char *key)
{
    /* find the index of the key in the eeprom_data array */
    int i = 0;
    while (eeprom_data[i].id != NULL)
    {
        if (eeprom_data[i].id == key)
        {
            return i;
        }

        i++;
    }
    return -1;
}

char* eeprom_get_key(int index)
{
    /* find the key at the index in the eeprom_data array */
    int i = 0;
    while (eeprom_data[i].id != NULL)
    {
        if (eeprom_data[i].address == index)
        {
            return eeprom_data[i].id;
        }

        i++;
    }
    return NULL;
}

bool eeprom_read_data_key(char *key, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }
    /* read data from eeprom given key and size */
    int address = eepromGetAddress(key);
    eeprom_read(address, data, size);

}

bool eeprom_read_data_index(uint16_t address, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }
    /* read data from eeprom given index */
    eeprom_read(address, data, size);
    //EEPROM.get(index, data); // TODO will need update with new eeprom driver
}

bool eeprom_write_data_key(char *key, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }
    /* write data to eeprom given key, offset, and size of data */
    int address = eepromGetAddress(key);
    eeprom_write(address, data, size);
}

bool eeprom_write_data_index(uint16_t address, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }
    /* write data to eeprom given page, offset, and size of data */
    eeprom_write(address, data, size);
}
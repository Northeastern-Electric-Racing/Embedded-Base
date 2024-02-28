#include "eepromdirectory.h"
#include "general/include/m24c32.h"


/*   user defined data type for taking in
  1. id of each fault
  2. size of the fault
 */


/* function definition for entering parameters of the fault coniditon
passing the array of struct to the function */
void eeprom_init(struct eeprom_partition partition_list[], int array_size)
{
    eeprom_data[0].address = 0;
    eeprom_data[0].size = partition_list[0].size;
    eeprom_data[0].id = partition_list[0].id;

    for (int i = 1; i < array_size; i++) {
        eeprom_data[i].id = partition_list[i].id;
        eeprom_data[i].size;

        // calculating the address through the logic defined in shepherd
        eeprom_data[i].address = eeprom_data[i-1].size + eeprom-data[i-1].address;
    }
}

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


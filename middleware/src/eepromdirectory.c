#include "eepromdirectory.h"
#include "general/include/m24c32.h"

void eeprom_init(eepromfs_t *eeprom, eeprom_read read_cb, eeprom_write write_cb, eeprom_delete delete_cb)
{
    eeprom->write_cb = write_cb;
    eeprom->read_cb = read_cb;
    eeprom->delete_cb = delete_cb;
    eeprom->eeprom_data = NULL;
}

void eeprom_partition(eepromfs_t *eeprom, eeprom_partition_t partition_list[], int array_size)
{
    eeprom->eeprom_data[0].address = 0;
    eeprom->eeprom_data[0].size = partition_list[0].size;
    eeprom->eeprom_data[0].id = partition_list[0].id;

    for (uint8_t i = 1; i < array_size; i++) {
        eeprom->eeprom_data[i].id = partition_list[i].id;
        eeprom->eeprom_data[i].size;

        // calculating the address through the logic defined in shepherd
        eeprom->eeprom_data[i].address = eeprom_data[i-1].size + eeprom-data[i-1].address;
    }
}

uint16_t eeprom_get_address(eepromfs_t *eeprom, char *key)
{
    /* find the index of the key in the eeprom_data array */
    int i = 0;
    while (eeprom->eeprom_data[i].id != NULL)
    {
        if (eeprom->eeprom_data[i].id == key)
        {
            return eeprom->eeprom_data[i].address;
        }

        i++;
    }
    return -1;
}

uint16_t eeprom_get_index(eepromfs_t *eeprom, char *key)
{
    /* find the index of the key in the eeprom_data array */
    int i = 0;
    while (eeprom->eeprom_data[i].id != NULL)
    {
        if (eeprom->eeprom_data[i].id == key)
        {
            return i;
        }

        i++;
    }
    return -1;
}

char* eeprom_get_key(eepromfs_t *eeprom, int index)
{
    /* find the key at the index in the eeprom_data array */
    int i = 0;
    while (eeprom->eeprom_data[i].id != NULL)
    {
        if (eeprom->eeprom_data[i].address == index)
        {
            return eeprom->eeprom_data[i].id;
        }

        i++;
    }
    return NULL;
}

bool eeprom_read_data_key(eepromfs_t *eeprom, char *key, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }

    /* read data from eeprom given key and size */
    int address = eepromGetAddress(key);
    eeprom->read_cb(address, data, size);
}

bool eeprom_read_data_index(eepromfs_t *eeprom, uint16_t address, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }

    /* read data from eeprom given index */
    eeprom->read_cb(address, data, size);
}

bool eeprom_write_data_key(eepromfs_t *eeprom, char *key, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }

    /* write data to eeprom given key, offset, and size of data */
    int address = eepromGetAddress(key);
    eeprom->write_cb(address, data, size);
}

bool eeprom_write_data_index(eepromfs_t *eeprom, uint16_t address, void *data, uint16_t size)
{
    if(!data) {
        return false;
    }

    /* write data to eeprom given page, offset, and size of data */
    eeprom->write_cb(address, data, size);
}


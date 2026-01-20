#include "eeprom_alloc.h"

#define byte_index(n)	((n) / 8)
#define bit_index(n)	((n) % 8)
#define nth_bit_mask(n) (1 << (n))

static int _get_alloc_table(uint8_t *table, uint16_t id)
{
	return (table[byte_index(id)] >> bit_index(id)) & 1;
}

static void _put_alloc_table(uint8_t *table, uint16_t id, uint8_t val)
{
	uint8_t bit_mask = nth_bit_mask(bit_index(id));

	if (val) {
		table[byte_index(id)] |= bit_mask;
	} else {
		bit_mask = ~bit_mask;
		table[byte_index(id)] &= bit_mask;
	}
}

static eeprom_status_t _update_eeprom_alloc_table(eeprom_directory_t *directory,
						  uint16_t id)
{
	uint16_t data_index = byte_index(id);
	m24c32_t *device = directory->device;
	uint16_t addr = ALLOC_TABLE_BEGIN + data_index;
	uint8_t *data = directory->alloc_table + data_index;

	return m24c32_write(device, addr, data, 1);
}

eeprom_status_t _init_alloc_table(eeprom_directory_t *directory)
{
	m24c32_t *device = directory->device;

	return m24c32_read(device, ALLOC_TABLE_BEGIN, directory->alloc_table,
			   ALLOC_TABLE_SIZE);
}

void _print_alloc_table(eeprom_directory_t *directory)
{
	uint8_t *alloc_table = directory->alloc_table;
	for (int id = 0; id < BLOCK_COUNT; id++) {
		putchar(_get_alloc_table(alloc_table, id) ? '1' : '0');

		if ((id + 1) % 64 == 0) {
			putchar('\n');
		} else if ((id + 1) % 8 == 0) {
			putchar(' ');
		}
	}
	putchar('\n');
}

uint16_t _alloc_block(eeprom_directory_t *directory)
{
	uint8_t *alloc_table = directory->alloc_table;

	for (uint16_t id = 0; id < BLOCK_COUNT; id++) {
		if (_get_alloc_table(alloc_table, id) == 0) {
			_put_alloc_table(alloc_table, id, 1);
			eeprom_status_t ret =
				_update_eeprom_alloc_table(directory, id);
			if (ret != EEPROM_OK) {
				printf("ERROR: failed to update eeprom alloc table.\n");
				_put_alloc_table(alloc_table, id, 0);
				return BLOCK_COUNT;
			}
			return id;
		}
	}
	return BLOCK_COUNT;
}

void _free_block(eeprom_directory_t *directory, uint16_t *ids, uint8_t size)
{
	uint8_t *alloc_table = directory->alloc_table;

	for (int i = 0; i < size; i++) {
		uint16_t id = ids[i];
		_put_alloc_table(alloc_table, id, 0);
		eeprom_status_t ret = _update_eeprom_alloc_table(directory, id);
		if (ret != EEPROM_OK) {
			printf("ERROR: failed to update eeprom allo table.\n");
		}
	}
}

int _is_allocated(eeprom_directory_t *directory, uint16_t id)
{
	uint8_t *alloc_table = directory->alloc_table;

	return _get_alloc_table(alloc_table, id);
}

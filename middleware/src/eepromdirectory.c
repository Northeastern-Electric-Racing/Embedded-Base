#include "eepromdirectory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BASE_ADDR 0

size_t directory_init(eeprom_directory_t *directory,
		      const struct partition_cfg partitions[],
		      size_t num_partitions)
{
	assert(directory);
	assert(partitions);
	assert(num_partitions);

	directory->partitions =
		malloc(sizeof(struct partition_cfg) * num_partitions);

	/* Accumulator that gives the address of the start of the partition */
	size_t addr_acc = BASE_ADDR;

	for (int i = 0; i < num_partitions; i++) {
		directory->partitions[i] = partitions[i];
		directory->partitions[i].address = addr_acc;
		directory->partitions[i].head_address = addr_acc;
		addr_acc += partitions[i].size;
	}

	directory->num_partitions = num_partitions;

	return addr_acc;
}

int32_t eeprom_get_base_address(const eeprom_directory_t *directory,
				const char *key)
{
	assert(directory);
	assert(key);

	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			return directory->partitions[i].address;
		}
	}

	return -1;
}

int32_t eeprom_get_head_address(const eeprom_directory_t *directory,
				const char *key)
{
	assert(directory);
	assert(key);

	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			return directory->partitions[i].head_address;
		}
	}

	return -1;
}

size_t eeprom_get_size(const eeprom_directory_t *directory, const char *key)
{
	assert(directory);
	assert(key);

	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			return directory->partitions[i].size;
		}
	}

	return 0;
}
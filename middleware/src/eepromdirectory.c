#include "eepromdirectory.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BASE_ADDR 0

struct partition {
	const struct partition_cfg cfg;
	size_t start_address; /* Address of the beginning of the partition */
	size_t head_address; /* The current read/write head address */
};

struct eeprom_directory {
	struct partition *partitions;
	size_t num_partitions;
};

void directory_init(struct eeprom_directory *directory,
		    const struct partition_cfg partitions[],
		    size_t num_partitions)
{
	assert(directory);
	assert(partitions);
	assert(num_partitions);

	/* Accumulator that gives the address of the start of the partition */
	size_t addr_acc = BASE_ADDR;

	directory->partitions =
		malloc(sizeof(struct partition) * num_partitions);

	for (int i = 0; i < num_partitions; i++) {
		directory->partitions[i].cfg.id = partitions[i].id;
		directory->partitions[i].cfg.size = partitions[i].size;

		directory->partitions[i].start_address = addr_acc;
		directory->partitions[i].head_address =
			directory->partitions[i].start_address;

		addr_acc += directory->partitions[i].size + 1;
	}

	return addr_acc - 1;
}

signed long eeprom_get_base_address(const struct eeprom_directory *directory,
				    const char *key)
{
	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			return directory->partitions[i].start_address;
		}
	}

	return -1;
}

signed long eeprom_get_head_address(const struct eeprom_directory *directory,
				    const char *key)
{
	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			return directory->partitions[i].head_address;
		}
	}

	return -1;
}

signed long eeprom_get_size(const struct eeprom_directory *directory,
			    const char *key)
{
	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			return directory->partitions[i].head_address;
		}
	}

	return -1;
}

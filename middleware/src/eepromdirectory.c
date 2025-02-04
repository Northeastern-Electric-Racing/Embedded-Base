#include "eepromdirectory.h"
#include <stdlib.h>
#include <string.h>

struct partition {
	const char *id; /*  key  */
	size_t size; /* bytes */
	size_t start_address; /* Address of the beginning of the partition */
	size_t head_address; /* The current read/write head address */
};

struct eeprom_directory {
	struct partition *partitions;
	size_t num_partitions;
};

void eeprom_init(struct eeprom_directory *directory,
		 struct partition partitions[], size_t num_partitions)
{
	/* Size accumulator */
	size_t size_acc;

	partitions[0].start_address = 0;
	partitions[0].head_address = partitions[0].start_address;
	size_acc = partitions[0].size;

	for (int i = 1; i < num_partitions; i++) {
		partitions[i].start_address = size_acc;
		partitions[i].head_address = partitions[i].start_address;
		size_acc += partitions[i].size;
	}

	directory->partitions =
		malloc(sizeof(struct partition) * num_partitions);
	memcpy(directory->partitions, partitions,
	       sizeof(struct partition) * num_partitions);

	directory->num_partitions = num_partitions;
}

signed long eeprom_get_start_address(const struct eeprom_directory *directory,
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

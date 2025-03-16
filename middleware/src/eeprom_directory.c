#include "eeprom_directory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BASE_ADDR 0

eeprom_status_t directory_init(eeprom_directory_t *directory,
			       const struct partition_cfg partitions[],
			       size_t num_partitions)
{
	if ((directory == NULL) || (partitions == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	directory->partitions =
		malloc(sizeof(struct partition_cfg) * num_partitions);

	if (directory->partitions == NULL) {
		return EEPROM_ERROR_ALLOCATION;
	}

	/* Accumulator that gives the address of the start of the partition */
	size_t addr_acc = BASE_ADDR;

	for (int i = 0; i < num_partitions; i++) {
		directory->partitions[i] = partitions[i];
		directory->partitions[i].address = addr_acc;
		directory->partitions[i].head_address = addr_acc;
		addr_acc += partitions[i].size;
	}

	directory->num_partitions = num_partitions;

	return EEPROM_OK;
}

eeprom_status_t eeprom_get_base_address(const eeprom_directory_t *directory,
					const char *key, uint16_t *address)
{
	if ((directory == NULL) || (key == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			*address = directory->partitions[i].address;
			return EEPROM_OK;
		}
	}

	return EEPROM_ERROR_NOT_FOUND;
}

eeprom_status_t eeprom_get_head_address(const eeprom_directory_t *directory,
					const char *key, uint16_t *address)
{
	if ((directory == NULL) || (key == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			*address = directory->partitions[i].head_address;
			return EEPROM_OK;
		}
	}

	return EEPROM_ERROR_NOT_FOUND;
}

eeprom_status_t eeprom_get_size(const eeprom_directory_t *directory,
				const char *key, uint16_t *size)
{
	if ((directory == NULL) || (key == NULL)) {
		return EEPROM_ERROR_NULL_POINTER;
	}

	for (int i = 0; i < directory->num_partitions; i++) {
		if (strcmp(directory->partitions[i].id, key) == 0) {
			*size = directory->partitions[i].size;
			return EEPROM_OK;
		}
	}

	return EEPROM_ERROR_NOT_FOUND;
}
/**
 * @file eeprom_directory.h
 * @brief EEPROM Directory Management
 * 
 * This file provides functions to initialize and manage an EEPROM directory with partitions.
 * Functions return `eeprom_status_t` error codes, which are defined in `eeprom_status.h`.
 * 
 */

#ifndef EEPROM_DIRECTORY_H
#define EEPROM_DIRECTORY_H

#include "eeprom_status.h"
#include <stddef.h>
#include <stdint.h>

struct partition_cfg {
	const char *id; /* The ID of the partition */
	uint16_t size; /* The size of the partition in bytes */
	uint16_t address;
	uint16_t head_address;
};

typedef struct {
	struct partition_cfg *partitions;
	size_t num_partitions;
} eeprom_directory_t;

/**
 * @brief Initializes an EEPROM directory with partition IDs and sizes.
 * 
 * This function sets up partition configurations within the EEPROM memory.
 * The total allocated address space is calculated based on the provided partitions.
 *
 * @param directory Pointer to the EEPROM directory structure.
 * @param partitions Array of partition configurations, including IDs and sizes.
 * @param num_partitions Number of partitions to create.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code on failure. 
 */
eeprom_status_t directory_init(eeprom_directory_t *directory,
			       const struct partition_cfg partitions[],
			       size_t num_partitions);

/**
 * @brief Get the address of the beginning of a partition.
 *
 * This function searches for a partition by its name and returns its starting address.
 * 
 * @param directory Pointer to the EEPROM directory structure.
 * @param key Name of the partition.
 * @param address Pointer to store the retrieved base address.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code if the partition is not found.
 */
eeprom_status_t eeprom_get_base_address(const eeprom_directory_t *directory,
					const char *key, uint16_t *address);

/**
 * @brief Get the current read/write head of a partition.
 *
 * This function provides the address of the next location available for writing within the partition.
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of the partition.
 * @param address Pointer to store the retrieved head address.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code if the partition is not found.
 */
eeprom_status_t eeprom_get_head_address(const eeprom_directory_t *directory,
					const char *key, uint16_t *address);

/**
 * @brief Get the size of a partition
 *
 * This function returns the allocated size of a specific partition in bytes.
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of a partition.
 * @param size Pointer to store the partition size.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code if the partition is not found.	
 */
eeprom_status_t eeprom_get_size(const eeprom_directory_t *directory,
				const char *key, uint16_t *size);

#endif // EEPROM_DIRECTORY_H
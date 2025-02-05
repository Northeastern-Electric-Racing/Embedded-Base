#ifndef EEPROMDIRECTORY_H
#define EEPROMDIRECTORY_H

#include <stdint.h>
#include <stddef.h>

typedef struct eeprom_directory eeprom_directory;

/**
 * @brief Initiailze an EEPROM directory with partition IDs and sizes.
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param keys Array of IDs by which EEPROM partitions will be addressed.
 * @param sizes The size of each partition.
 * @param num_partitions The number of partitions to create.
 */
void directory_init(struct eeprom_directory *directory, const char *IDs[],
		    const size_t sizes[], size_t num_partitions);

/**
 * @brief Get the address of the beginning of a partition.
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of partition
 * @return signed long Start address, or -1 if key not found.
 */
int32_t eeprom_get_base_address(const struct eeprom_directory *directory,
				const char *key);

/**
 * @brief Get the current read/write head of a partition.
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of the partition.
 * @return signed long The r/w head address, or -1 if key not found.
 */
int32_t eeprom_get_head_address(const struct eeprom_directory *directory,
				const char *key);

/**
 * @brief Get the size of a partition
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of a partition.
 * @return size_t The size of the partiton, or -1 if key not found.
 */
int32_t eeprom_get_size(const struct eeprom_directory *directory,
			const char *key);

#endif // EEPROMDIRECTORY_H
#ifndef EEPROMDIRECTORY_H
#define EEPROMDIRECTORY_H

#include <stdint.h>
#include <stddef.h>


struct partition_cfg {
	uint32_t *id; /* The ID of the partition */
	size_t size; /* The size of the partition in bytes */
};

typedef struct eeprom_directory eeprom_directory;

/**
 * @brief Initiailze an EEPROM directory with partition IDs and sizes.
 * 
 * @param directory Pointer to EEPROM directory struct.
 * @param partitions Array of partition names and sizes.
 * @param num_partitions The number of partitions to create.
 * @return size_t The size of the partitions in Bytes.
 */
size_t directory_init(struct eeprom_directory *directory,
		    const struct partition_cfg partitions[], size_t num_partitions);

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
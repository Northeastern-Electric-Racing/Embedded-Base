#ifndef EEPROMDIRECTORY_H
#define EEPROMDIRECTORY_H

#include <stddef.h>
#include <stdint.h>

struct partition_cfg {
	const char *id; /* The ID of the partition */
	size_t size; /* The size of the partition in bytes */
	size_t address;
	size_t head_address;
};

typedef struct {
	struct partition_cfg *partitions;
	size_t num_partitions;
} eeprom_directory_t;

/**
 * @brief Initiailze an EEPROM directory with partition IDs and sizes.
 *
 * @param directory Pointer to EEPROM directory struct.
 * @param partitions Array of partition names and sizes.
 * @param num_partitions The number of partitions to create.
 * @return size_t The total allocated address space.
 */
size_t directory_init(eeprom_directory_t *directory,
		      const struct partition_cfg partitions[],
		      size_t num_partitions);

/**
 * @brief Get the address of the beginning of a partition.
 *
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of partition
 * @return int32_t Start address, or -1 if key not found.
 */
int32_t eeprom_get_base_address(const eeprom_directory_t *directory,
				const char *key);

/**
 * @brief Get the current read/write head of a partition.
 *
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of the partition.
 * @return int32_t The r/w head address, or -1 if key not found.
 */
int32_t eeprom_get_head_address(const eeprom_directory_t *directory,
				const char *key);

/**
 * @brief Get the size of a partition
 *
 * @param directory Pointer to EEPROM directory struct.
 * @param key Name of a partition.
 * @return size_t The size of the partiton, or 0 if key not found.
 */
size_t eeprom_get_size(const eeprom_directory_t *directory, const char *key);

#endif // EEPROMDIRECTORY_H
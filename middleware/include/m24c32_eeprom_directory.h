#pragma once

#include "m24c32.h"
#include "eepromdirectory.h"

/**
 * @brief Write to a section of the EEPROM. Writes that overflow the size of the partition
 * will wrap to the head of the partition.
 * 
 * @param eeprom Pointer to eeprom datastruct.
 * @param id ID of eeprom partition to write to.
 * @param data Data to write.
 * @param len Length of data to write in Bytes.
 * @param offset Offset from partition base address in Bytes.
 * @return int Error code. -1 if id is not a valid partition ID, -2 if offset is invalid, -3 if attempting to write
 * into the next partition.
 */
int m24c32_directory_write(const eeprom_directory_t *directory,
			   m24c32_t *eeprom, const char *id, uint8_t *data,
			   size_t len, size_t offset);

/**
 * @brief Read a section of the EEPROM. Reads that overflow the size of the partition
 * will wrap to the head of the partition.
 * 
 * @param eeprom Pointer to eeprom datastruct.
 * @param id ID of eeprom partition to write to.
 * @param data Read output.
 * @param len Length of data to write in Bytes.
 * @param offset Offset from partition base address in Bytes.
 * @return int Error code. -1 if id is not a valid partition ID, -2 if offset is invalid, -3 if attempting to read
 * into the next partition.
 */
int m24c32_directory_read(const eeprom_directory_t *directory, m24c32_t *eeprom,
			  const char *id, uint8_t *data, size_t len,
			  size_t offset);
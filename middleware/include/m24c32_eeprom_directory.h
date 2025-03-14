#pragma once

/**
 * @file m24c32_eeprom_directory.h
 * @brief EEPROM Partitioned Read/Write Interface
 * 
 * This file provides an interface for reading and writing to specific partitions 
 * within an EEPROM using a directory-based approach.
 * 
 */

#include "m24c32.h"
#include "eepromdirectory.h"

/**
 * @brief Write to a section of the EEPROM. Writes that overflow the size of the partition
 * will wrap to the head of the partition.
 * 
 * @param directory Pointer to the EEPROM directory structure.
 * @param eeprom Pointer to the EEPROM device structure.
 * @param id ID of eeprom partition to write to.
 * @param data Data to write.
 * @param len Length of data to write in Bytes.
 * @param offset Offset from partition base address in Bytes.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code on failure.
 */
eeprom_status_t m24c32_directory_write(const eeprom_directory_t *directory,
				       m24c32_t *eeprom, const char *id,
				       uint8_t *data, uint16_t len,
				       uint16_t offset);

/**
 * @brief Read a section of the EEPROM. Reads that overflow the size of the partition
 * will wrap to the head of the partition.
 * 
 * @param directory Pointer to the EEPROM directory structure.
 * @param eeprom Pointer to the EEPROM device structure.
 * @param id ID of eeprom partition to read from.
 * @param data Read output.
 * @param len Length of data to write in Bytes.
 * @param offset Offset from partition base address in Bytes.
 * @return eeprom_status_t Returns EEPROM_OK on success or an error code on failure.
 */
eeprom_status_t m24c32_directory_read(const eeprom_directory_t *directory,
				      m24c32_t *eeprom, const char *id,
				      uint8_t *data, uint16_t len,
				      uint16_t offset);
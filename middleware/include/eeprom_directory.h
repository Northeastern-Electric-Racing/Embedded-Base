/**
 * @file eeprom_directory.h
 * @author Sogo Nishihara (sogonishi@gmail.com)
 * @brief EEPROM Directory Management API
 *
 * This file defines the public API for a simple key-value directory
 * stored on EEPROM device.
 *
 * The directory provides block-based persistent storage, managing
 * allocation, lookup, insertion, and deletion of values associated
 * with fixed-size (4-byte) keys.
 *
 * All functions return eeprom_status_t error codes defined in
 * eeprom_status.h. */

#ifndef EEPROM_DIRECTORY_H
#define EEPROM_DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eeprom_alloc.h"
#include "eeprom_directory_struct.h"
#include "eeprom_status.h"
#include "eeprom_storage.h"
#include "m24c32.h"

/**
 * @brief Initialize the EEPROM directory structure.
 *
 * This function initializes the directory structure with the provided device
 * interface, loads the allocation table and key map from EEPROM memory.
 *
 * @param device Pointer to the M24C32 device interface structure.
 * @param directory Pointer to the directory structure to initialize. Must be
 * allocated by caller.
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_NULL_POINTER If device or directory is NULL.
 * @retval EEPROM_ERROR If initialization of alloc table or storage fails.
 */
eeprom_status_t directory_init(m24c32_t *device, eeprom_directory_t *directory);

/**
 * @brief Retrieve a value from the directory by key.
 *
 * This function looks up a key in the directory and retrieves the associated
 * data. The output buffer is allocated by this function and must be freed by
 * the caller.
 *
 * @param directory Pointer to the initialized directory structure.
 * @param key Pointer to a 4-byte key (not null-terminated).
 * @param out Pointer to a pointer that will receive the allocated output
 * buffer. The caller is responsible for freeing this buffer.
 * @param out_size Pointer to store the size of the output data in bytes.
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_NULL_POINTER If directory, key, out, or out_size is
 * NULL.
 * @retval EEPROM_ERROR_NOT_FOUND If the key does not exist in the directory.
 * @retval EEPROM_ERROR_ALLOCATION If memory allocation fails.
 */
eeprom_status_t get_directory_value(eeprom_directory_t *directory,
				    const uint8_t *key, uint8_t **out,
				    uint16_t *out_size);

/**
 * @brief Store a value in the directory with the specified key.
 *
 * This function stores data in the directory. If the key already exists, the
 * old value is deleted first. The data is stored in blocks of BLOCK_SIZE bytes,
 * with remaining bytes padded with zeros.
 *
 * @param directory Pointer to the initialized directory structure.
 * @param key Pointer to a 4-byte key (not null-terminated).
 * @param value Pointer to the data to store.
 * @param value_size Size of the data in bytes (maximum 16 bytes, i.e., 4
 * blocks).
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_NULL_POINTER If directory or value is NULL.
 * @retval EEPROM_ERROR If value_size is 0.
 * @retval EEPROM_ERROR_TOO_BIG If value_size exceeds 16 bytes (4 blocks).
 * @retval EEPROM_ERROR_ALLOCATION If block allocation fails.
 */
eeprom_status_t set_directory_value(eeprom_directory_t *directory,
				    const uint8_t *key, uint8_t *value,
				    const uint16_t value_size);

/**
 * @brief Delete a value from the directory by key.
 *
 * This function removes a key-value pair from the directory and frees the
 * associated blocks. The blocks are marked as available for reuse.
 *
 * @param directory Pointer to the initialized directory structure.
 * @param key Pointer to a 4-byte key (not null-terminated).
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_NULL_POINTER If directory or key is NULL.
 * @retval EEPROM_ERROR_NOT_FOUND If the key does not exist in the directory.
 */
eeprom_status_t delete_directory_value(eeprom_directory_t *directory,
				       const uint8_t *key);

#endif // EEPROM_DIRECTORY_H
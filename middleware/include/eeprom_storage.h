/**
 * @file eeprom_storage.h
 * @author Sogo Nishihara (sogonishi@gmail.com)
 * @brief EEPROM Storage Management
 *
 * This file provides functions for managing data storage in the EEPROM
 * directory. Functions handle reading, writing, and deleting data blocks
 * associated with block IDs.
 *
 */

#ifndef EEPROM_STORAGE
#define EEPROM_STORAGE

#include <stdlib.h>

#include "eeprom_alloc.h"
#include "eeprom_directory_struct.h"

/**
 * @brief Initialize storage by loading key-map table from EEPROM memory.
 *
 * This function reads the key-map table from EEPROM and populates the
 * directory's key_map structure in memory.
 *
 * @param directory Pointer to the initialized directory structure.
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_ALLOCATION If memory allocation fails.
 * @retval EEPROM_ERROR If EEPROM read operation fails.
 */
eeprom_status_t init_storage(eeprom_directory_t *directory);

/**
 * @brief Retrieve data from EEPROM using block IDs.
 *
 * This function reads data from multiple blocks in EEPROM and allocates a
 * buffer to store the result. The caller is responsible for freeing the output
 * buffer.
 *
 * @param directory Pointer to the initialized directory structure.
 * @param ids Array of block IDs (up to 4, terminated by BLOCK_COUNT).
 * @param out Pointer to a pointer that will receive the allocated output
 * buffer. The caller must free this buffer.
 * @param out_size Pointer to store the size of the output data in bytes.
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_ALLOCATION If memory allocation fails.
 * @retval EEPROM_ERROR_NOT_FOUND If any block ID is not allocated.
 * @retval EEPROM_ERROR If EEPROM read operation fails.
 */
eeprom_status_t get_data(eeprom_directory_t *directory, const uint16_t *ids,
                         uint8_t **out, uint16_t *out_size);

/**
 * @brief Store data in EEPROM using block IDs.
 *
 * This function writes data to multiple blocks in EEPROM. Each block is
 * BLOCK_SIZE bytes. If the data size is smaller than the total block size,
 * remaining bytes are padded with zeros.
 *
 * @param directory Pointer to the initialized directory structure.
 * @param ids Array of block IDs (up to 4, terminated by BLOCK_COUNT).
 * @param value Pointer to the data to write.
 * @param value_size Size of the data in bytes.
 *
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on
 * failure.
 *
 * @retval EEPROM_ERROR_ALLOCATION If memory allocation for block buffer fails.
 * @retval EEPROM_ERROR If EEPROM write operation fails.
 */
eeprom_status_t put_data(eeprom_directory_t *directory, const uint16_t *ids,
                         uint8_t *value, uint16_t value_size);

/**
 * @brief Delete data blocks from EEPROM.
 *
 * This function frees the blocks associated with the given IDs, making them
 * available for reuse. The blocks are marked as free in the allocation table.
 *
 * @param directory Pointer to the initialized directory structure.
 * @param ids Array of block IDs to free (up to 4, terminated by BLOCK_COUNT).
 *
 * @return eeprom_status_t Returns EEPROM_OK on success.
 */
eeprom_status_t delete_data(eeprom_directory_t *directory, uint16_t *ids);

#endif

/**
 * @file eeprom_alloc.h
 * @author Sogo Nishihara (sogonishi@gmail.com)
 * @brief EEPROM Block Allocation Management
 * @version 0.1
 * @date 2026-01-02
 * 
 * This file provides functions for managing data block allocation in the EEPROM.
 * It maintains a bit-vector allocation table to track which blocks are in use.
 * 
 */

#ifndef EEPROM_ALLOC_H
#define EEPROM_ALLOC_H

#include <stdio.h>

#include "eeprom_directory_struct.h"

/**
 * @brief Initialize allocation table by loading it from EEPROM memory.
 * 
 * This function reads the allocation table from EEPROM and populates the directory's
 * alloc_table structure in memory.
 * 
 * @param directory Pointer to the initialized directory structure.
 * 
 * @return eeprom_status_t Returns EEPROM_OK on success, or an error code on failure.
 * 
 * @retval EEPROM_ERROR If EEPROM read operation fails.
 */
eeprom_status_t init_alloc_table(eeprom_directory_t *directory);

/**
 * @brief Print the allocation table in a human-readable format.
 * 
 * This function prints the allocation table showing which blocks are allocated (1)
 * and which are free (0). The output is formatted with 8 bits per group and
 * 64 bits per line.
 * 
 * @param directory Pointer to the initialized directory structure.
 */
void print_alloc_table(eeprom_directory_t *directory);

/**
 * @brief Allocate a single block from the EEPROM.
 * 
 * This function finds the first free block in the allocation table, marks it as
 * allocated in both local memory and EEPROM, and returns its ID.
 * 
 * @param directory Pointer to the initialized directory structure.
 * 
 * @return uint16_t Returns the allocated block ID on success, or BLOCK_COUNT if allocation fails.
 * 
 * @retval BLOCK_COUNT If no free blocks are available or EEPROM update fails.
 */
uint16_t alloc_block(eeprom_directory_t *directory);

/**
 * @brief Free one or more blocks in the EEPROM.
 * 
 * This function marks the specified blocks as free in both local memory and EEPROM.
 * If a block is already freed, this function does nothing for that block.
 * 
 * @param directory Pointer to the initialized directory structure.
 * @param ids Array of block IDs to free.
 * @param size Number of block IDs in the array.
 */
void free_block(eeprom_directory_t *directory, uint16_t *ids, uint8_t size);

/**
 * @brief Check if a block is allocated.
 * 
 * This function checks the allocation table to determine if a specific block is
 * currently allocated or free.
 * 
 * @param directory Pointer to the initialized directory structure.
 * @param id Block ID to check.
 * 
 * @return int Returns 1 if the block is allocated, 0 if it is free.
 */
int is_allocated(eeprom_directory_t *directory, uint16_t id);

#endif

/**
 * @file eeprom_directory_struct.h
 * @author Sogo Nishihara (sogonishi@gmail.com)
 * @brief EEPROM Directory Data Structures and Constants
 * @version 0.1
 * @date 2026-01-02
 *
 * This file defines the data structures and constants used for the EEPROM
 * directory system. It includes memory layout definitions, block allocation
 * parameters, and structure definitions.
 *
 * EEPROM Memory Layout (Total: 4096 bytes)
 *
 *  Address
 *  0x0000
 *  +--------------------------------------------------+
 *  | Allocation Table                                 |
 *  |  - Bit map (1 bit per data block)                |
 *  |  - Size : ALLOC_TABLE_SIZE bytes                 |
 *  |  - Blocks : BLOCK_COUNT                          |
 *  +--------------------------------------------------+
 *  | Key Map Table                                    |
 *  |  - Array of directory_key_map_t                  |
 *  |  - Entry size : KEY_MAP_STRUCT_SIZE bytes        |
 *  |  - Entry count: KEY_MAP_COUNT                    |
 *  |  - Total size : KEY_MAP_SIZE bytes               |
 *  +--------------------------------------------------+
 *  | Data Space                                       |
 *  |  - Data blocks                                   |
 *  |  - Block size : BLOCK_SIZE bytes                 |
 *  |  - Block count: BLOCK_COUNT                      |
 *  |                                                  |
 *  |  +------------------+                            |
 *  |  | Block 0          |                            |
 *  |  +------------------+                            |
 *  |  | Block 1          |                            |
 *  |  +------------------+                            |
 *  |  | ...              |                            |
 *  |  +------------------+                            |
 *  |  | Block N          |                            |
 *  |  +------------------+                            |
 *  +--------------------------------------------------+
 *  0x0FFF
 */

#ifndef EEPROM_DIRECTORY_STRUCT
#define EEPROM_DIRECTORY_STRUCT

/** @brief Size of a single key-map entry structure in bytes (4-byte key +
 * 8-byte IDs array). */
#define KEY_MAP_STRUCT_SIZE 12

/** @brief Maximum number of key-value pairs that can be stored in the
 * directory. */
#define KEY_MAP_COUNT 128

/** @brief Size of a data block in bytes. */
#define BLOCK_SIZE 4

/** @brief Total number of data blocks available in the EEPROM. */
#define BLOCK_COUNT 512

/** @brief Starting address of the allocation table in EEPROM. */
#define ALLOC_TABLE_BEGIN 0

/** @brief Size of the allocation table in bytes (one bit per block). */
#define ALLOC_TABLE_SIZE (BLOCK_COUNT / 8)

/** @brief Starting address of the key-map table in EEPROM. */
#define KEY_MAP_BEGIN (ALLOC_TABLE_BEGIN + ALLOC_TABLE_SIZE)

/** @brief Total size of the key-map table in bytes. */
#define KEY_MAP_SIZE (KEY_MAP_STRUCT_SIZE * KEY_MAP_COUNT)

/** @brief Starting address of the data space in EEPROM. */
#define DATA_SPACE_BEGIN (KEY_MAP_BEGIN + KEY_MAP_SIZE)

/** @brief Total size of the EEPROM in bytes. */
#define EEPROM_SIZE 4096

#include <stdint.h>

#include "m24c32.h"

/**
 * @brief Key-value mapping structure.
 *
 * This structure maps a 4-byte key to up to 4 block IDs. Each block ID points
 * to a BLOCK_SIZE-byte data block in the EEPROM data space.
 */
typedef struct {
  uint8_t key[4]; /** 4-byte key (not null-terminated). */
  uint16_t
      ids[4]; /** Array of up to 4 block IDs (BLOCK_COUNT indicates end). */
} directory_key_map_t;

/**
 * @brief Main EEPROM directory structure.
 *
 * This structure contains all the state information for the EEPROM directory,
 * including the device interface, allocation table, and key map.
 */
typedef struct {
  m24c32_t *device; /** Pointer to the M24C32 device interface. */
  uint8_t alloc_table[ALLOC_TABLE_SIZE]; /** Bit-vector allocation table (one
                                            bit per block). */
  directory_key_map_t
      key_map[KEY_MAP_COUNT]; /** Array of key-value mappings. */
} eeprom_directory_t;

#endif
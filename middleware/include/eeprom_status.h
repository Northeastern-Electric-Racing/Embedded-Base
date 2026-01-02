/**
 * @file eeprom_status.h
 * @brief EEPROM Standardized Error Codes
 * 
 * This file defines the `eeprom_status_t` enumeration, which provides standardized 
 * error codes for all EEPROM-related operations.
 * 
 */

#ifndef EEPROM_STATUS
#define EEPROM_STATUS

typedef enum {
	EEPROM_OK = 0,
	EEPROM_ERROR = -1,
	EEPROM_ERROR_NOT_FOUND = -2,
	EEPROM_ERROR_OUT_OF_BOUNDS = -3,
	EEPROM_ERROR_ALIGNMENT = -4,
	EEPROM_ERROR_NULL_POINTER = -5,
	EEPROM_ERROR_ALLOCATION = -6,
	EEPROM_ERROR_COMMS = -7,
	EEPROM_ERROR_TOO_BIG = -8

} eeprom_status_t;

#endif
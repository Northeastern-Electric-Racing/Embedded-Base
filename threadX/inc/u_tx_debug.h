
#ifndef __U_TX_DEBUG_H
#define __U_TX_DEBUG_H

// clang-format off

#include <string.h>
#include <stdio.h>
#include "tx_api.h"
#include "stm32h5xx_hal.h"

/* General-purpose status macros. */
#define U_SUCCESS     0
#define U_ERROR	      1
#define U_QUEUE_EMPTY 2

/* Gets the name of the file. */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/* Debug message types. 
*  Specific types can by enabled/disabled by commenting/uncommenting their #define statements.
*  (i.e. if a #define is commented out, the associated messages will not be printed.)
*/
#define LOG_ERROR	/* Messages indicating that something has definitely gone wrong. */
#define LOG_WARNING	/* Messages indicating that something 'might' have gone wrong, but either isn't immidietely critical or is only a problem in certain contexts. */
#define LOG_INFO	/* Messages with miscellaneous/nice-to-have information. */

/* PRINTLN_ERROR() */
#ifdef LOG_ERROR
    #define PRINTLN_ERROR(message, ...) printf("[%s/%s()] ERROR: " message "\n", __FILENAME__, __func__, ##__VA_ARGS__) /* Prints an ERROR message in the format: "[file_name.c/function()] ERROR: {message}"*/
#else
    #define PRINTLN_ERROR(message, ...) /* If debugging is turned off, macro doesn't need to expand to anything. */
#endif

/* PRINTLN_WARNING() */
#ifdef LOG_WARNING
    #define PRINTLN_WARNING(message, ...) printf("[%s/%s()] WARNING: " message "\n", __FILENAME__, __func__, ##__VA_ARGS__) /* Prints a WARNING message in the format: "[file_name.c/function()] WARNING: {message}"*/
#else
    #define PRINTLN_WARNING(message, ...) /* If debugging is turned off, macro doesn't need to expand to anything. */
#endif

/* PRINTLN_INFO() */
#ifdef LOG_INFO
    #define PRINTLN_INFO(message, ...) printf("[%s/%s()] INFO: " message "\n", __FILENAME__, __func__, ##__VA_ARGS__) /* Prints an INFO message in the format: "[file_name.c/function()] INFO: {message}"*/
#else
    #define PRINTLN_INFO(message, ...) /* If debugging is turned off, macro doesn't need to expand to anything. */
#endif

/**
 * @brief Checks if a function is successful when called. DEBUG_PRINTLNs an error message if it fails.
 * @param function_call The function to call.
 * @param success The function's success code/macro (e.g., U_SUCCESS, TX_SUCCESS, etc.).
 * 
 * @note This macro intentionally doesn't support custom error messages, for the sake of readability. If an error is complex enough to 
 *       require a custom message, the error should probably be checked manually and DEBUG_PRINTLN() called directly.
 */
#define CATCH_ERROR(function_call, success) do { \
    int _function_status = (function_call); \
    if (_function_status != success) { \
        PRINTLN_ERROR("Function failed inside CATCH_ERROR() (Function: %s, Status: %d).", #function_call, _function_status); \
        return _function_status; \
    } \
} while(0)

/* Converts a ThreadX status macro to a printable string. */
/* This function is intended to be used with DEBUG_PRINTLN(), and shouldn't really ever be used outside of debugging purposes. */
/* (these macros are defined in tx_api.h) */
const char *tx_status_toString(UINT status);

/* Converts a STM32 HAL status macro to a printable string. */
/* This function is intended to be used with DEBUG_PRINTLN(), and shouldn't really ever be used outside of debugging purposes. */
/* (these macros are defined in stm32h5xx_hal_def.h) */
const char *hal_status_toString(HAL_StatusTypeDef status);

// clang-format on

#endif
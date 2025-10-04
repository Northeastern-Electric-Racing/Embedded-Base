#ifndef _U_TX_FLAGS_H
#define _U_TX_FLAGS_H

#include "tx_api.h"
#include "u_tx_debug.h"
#include <stdint.h>

extern TX_EVENT_FLAGS_GROUP event_flags;

/**
 * Initialize the project flag event group
 * Note: for our use-case there will be 
 * one event-group per project
 */
uint8_t init_flags();

/**
 * Sets a thread flag
 * @param flag thread flag to set
 */
uint8_t set_flag(ULONG flag);

/**
 * Blocks thread to receive a flag with the set timeout
 * @param flag to wait for
 * @param timeout time to wait for flag
 * TX_NO_WAIT and TX_WAIT_FOREVER
 */
uint8_t get_flag(ULONG flag, ULONG timeout);

#endif
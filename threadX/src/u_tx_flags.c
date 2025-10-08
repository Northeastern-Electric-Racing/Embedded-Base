#include "u_tx_flags.h"

TX_EVENT_FLAGS_GROUP event_flags;

uint8_t flags_init()
{
	if (tx_event_flags_create(&event_flags, "Thread Flags")) {
		DEBUG_PRINTLN("Failed to initialize flag event groups.");
		return U_ERROR;
	}
	return U_SUCCESS;
}

uint8_t set_flag(ULONG flag)
{
	if (tx_event_flags_set(&event_flags, flag, TX_OR)) {
		DEBUG_PRINTLN("Failed to set flag %d.", flag);
		return U_ERROR;
	}
	return U_SUCCESS;
}

uint8_t get_flag(ULONG flag, ULONG timeout)
{
	if (tx_event_flags_get(&event_flags, flag, TX_OR_CLEAR, NULL,
				timeout)) {
		DEBUG_PRINTLN("Failed to get flag %d.", flag);
		return U_ERROR;
	}
	return U_SUCCESS;
}
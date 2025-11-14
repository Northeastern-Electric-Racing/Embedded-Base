#include "u_tx_flags.h"

// clang-format off

TX_EVENT_FLAGS_GROUP event_flags;

uint8_t flags_init()
{
	if (tx_event_flags_create(&event_flags, "Thread Flags")) {
		PRINTLN_INFO("Failed to initialize flag event groups.");
		return U_ERROR;
	}
	PRINTLN_INFO("Ran flags_init().");
	return U_SUCCESS;
}

uint8_t set_flag(ULONG flag)
{
	if (tx_event_flags_set(&event_flags, flag, TX_OR)) {
		PRINTLN_INFO("Failed to set flag %d.", flag);
		return U_ERROR;
	}
	return U_SUCCESS;
}

uint8_t get_flag(ULONG flag, ULONG timeout)
{
	ULONG result_flags;
	if (tx_event_flags_get(&event_flags, flag, TX_OR_CLEAR, &result_flags,
				timeout)) {
		PRINTLN_INFO("Failed to get flag %d.", flag);
		return U_ERROR;
	}
	return U_SUCCESS;
}

// clang-format on
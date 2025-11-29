#include "u_tx_can.h"
#include "u_tx_debug.h"
#include "fdcan.h"

// clang-format off

uint8_t can_filter_init(FDCAN_HandleTypeDef *hcan, can_t *can,
			uint16_t standard_ids[2], uint32_t extended_ids[2])
{
	/* Init CAN interface */
	HAL_StatusTypeDef status = can_init(can, hcan);
	if (status != HAL_OK) {
		PRINTLN_ERROR("Failed to execute can_init() when initializing can1 (Status: %d/%s).", status, hal_status_toString(status));
		return U_ERROR;
	}

	/* Add filters for standard IDs */
	status = can_add_filter_standard(can, standard_ids);
	if (status != HAL_OK) {
		PRINTLN_ERROR("Failed to add standard filter to can1 (Status: %d/%s, ID1: %d, ID2: %d).", status, hal_status_toString(status), standard_ids[0], standard_ids[1]);
		return U_ERROR;
	}

	/* Add fitlers for extended IDs */
	status = can_add_filter_extended(can, extended_ids);
	if (status != HAL_OK) {
		PRINTLN_ERROR("Failed to add extended filter to can1 (Status: %d/%s, ID1: %ld, ID2: %ld).", status, hal_status_toString(status), extended_ids[0], extended_ids[1]);
		return U_ERROR;
	}

	("Ran can1_init().");

	return U_SUCCESS;
}

// clang-format on
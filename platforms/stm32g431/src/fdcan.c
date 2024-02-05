#include "fdcan.h"
#include <string.h>
#include <stdint.h>

/* NOTE: STM32G431 will have MAX of 3 CAN buses */
#define MAX_CAN_BUS	3

can_t *can_struct_list[MAX_CAN_BUS] = {NULL, NULL, NULL};

static can_callback_t find_callback(FDCAN_HandleTypeDef *hcan)
{
	for (uint8_t i = 0; i < MAX_CAN_BUS; i++) {
		if (hcan == can_struct_list[i]->hcan)
			return can_struct_list[i]->callback;
	}
	return NULL;
}

/* Add a CAN interfae to be searched for during the event of a callback */
static uint8_t add_interface(can_t *interface)
{
	for (uint8_t i = 0; i < MAX_CAN_BUS; i++) {
		/* Interface already added */
		if (interface->hcan == can_struct_list[i]->hcan)
			return -1;

		/* If empty, add interface */
		if (can_struct_list[i]->hcan == NULL) {
			can_struct_list[i] = interface;
			return 0;
		}
	}

	/* No open slots, something is wrong */
	return -2;
}

/* Run callback function when there a new message is received */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs)
{
	/* Handle CAN reception event */
	can_callback_t callback = find_callback(hcan);

	if (callback != NULL)
	{
		callback(hcan);
	}
}

HAL_StatusTypeDef can_init(can_t *can)
{
	/* set up filter */
	uint16_t high_id = can->id_list[0];
	uint16_t low_id = can->id_list[0];

	for (uint8_t i = 0; i < can->id_list_len; i++) {
		if (can->id_list[i] > high_id)
			high_id = can->id_list[i];
		if (can->id_list[i] < low_id)
			low_id = can->id_list[i];
	}

	uint32_t full_id = ((uint32_t)high_id << 16) | low_id;

	FDCAN_FilterTypeDef sFilterConfig;

	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
	sFilterConfig.FilterConfig = FDCAN_FILTER_DISABLE;
	sFilterConfig.FilterID1 = 0x0;
	sFilterConfig.FilterID2 = 0x7FF;

	uint8_t err = 0;
	err = HAL_FDCAN_ConfigFilter(can->hcan, &sFilterConfig);
	if (err != HAL_OK)
		return err;

	/* set up interrupt & activate CAN */
	err = HAL_FDCAN_Start(can->hcan);
	if (err != HAL_OK)
		return err;

	/* Override the default callback for FDCAN_IT_LIST_RX_FIFO0 */
	//err = HAL_FDCAN_ActivateNotification(can->hcan, FDCAN_IT_LIST_RX_FIFO0);
	err = add_interface(can);

	return err;
}

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
	FDCAN_TxHeaderTypeDef tx_header;
	tx_header.Identifier = msg->id;
	tx_header.IdType = FDCAN_STANDARD_ID;
	tx_header.TxFrameType = FDCAN_DATA_FRAME;
	tx_header.DataLength = msg->len;
	tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	tx_header.FDFormat = FDCAN_CLASSIC_CAN;
	//Function of below attributes is currently unknown.
	//tx_header.TxEventFifoControl = 
	//tx_header.MessageMarker = 

	uint32_t tx_mailbox;
	if (HAL_FDCAN_GetTxFifoFreeLevel(can->hcan) == 0)
		return HAL_BUSY;

	if (HAL_FDCAN_AddMessageToTxFifoQ(can->hcan, &tx_header, &msg->data))
		return HAL_ERROR;

	return HAL_OK;
}



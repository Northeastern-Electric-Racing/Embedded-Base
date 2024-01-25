#include "can.h"
#include <string.h>
#include <stdint.h>

/* NOTE: STM32F405 will have MAX of 3 CAN buses */
#define MAX_CAN_BUS	3

can_t *can_struct_list[MAX_CAN_BUS] = {NULL, NULL, NULL};

static can_callback_t find_callback(CAN_HandleTypeDef *hcan)
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

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	/* Handle CAN reception event */
	can_callback_t callback= find_callback(hcan);

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

	// uint32_t full_id = ((uint32_t)high_id << 16) | low_id;

	CAN_FilterTypeDef sFilterConfig;

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;   
	sFilterConfig.SlaveStartFilterBank = 14;

	// sFilterConfig.FilterBank = 0;                       /* Filter bank number (0 to 27 for most STM32 series) */
	// sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;   /* Identifier list mode */
	// sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  /* 32-bit identifier list */

	// sFilterConfig.FilterIdHigh = (full_id & 0xFFFF0000U) >> 5;
	// sFilterConfig.FilterIdLow = (full_id & 0xFFFFU) << 5;

	// sFilterConfig.FilterMaskIdHigh = 0xFFFF << 5;       /* Set to all ones for ID range */
	// sFilterConfig.FilterMaskIdLow = 0xFFFF;             /* Set to all ones for ID range */

	// sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  /* FIFO to assign the filter to */
	// sFilterConfig.FilterActivation = ENABLE;            /* Enable the filter */

	uint8_t err = 0;
	err = HAL_CAN_ConfigFilter(can->hcan, &sFilterConfig);
	if (err != HAL_OK)
		return err;

	/* set up interrupt & activate CAN */
	err = HAL_CAN_Start(can->hcan);
	if (err != HAL_OK)
		return err;

	/* Override the default callback for CAN_IT_RX_FIFO0_MSG_PENDING */
	err = HAL_CAN_ActivateNotification(can->hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	err = add_interface(can);

	return err;
}

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
	CAN_TxHeaderTypeDef tx_header;
	tx_header.StdId = msg->id;
	tx_header.ExtId = 0;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = msg->len;
	tx_header.TransmitGlobalTime = DISABLE;

	uint32_t tx_mailbox;
	if (HAL_CAN_GetTxMailboxesFreeLevel(can->hcan) == 0)
		return HAL_BUSY;

	if (HAL_CAN_AddTxMessage(can->hcan, &tx_header, msg->data, &tx_mailbox))
		return HAL_ERROR;

	return HAL_OK;
}

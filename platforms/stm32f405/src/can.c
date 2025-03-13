#include "can.h"
#include <stdint.h>
#include <string.h>

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

HAL_StatusTypeDef can_init(can_t *can)
{
	/* Reset filter bank */
	can->filter_bank = 0;

	/* set up interrupt & activate CAN */
	int8_t err = HAL_CAN_ActivateNotification(can->hcan,
						  CAN_IT_RX_FIFO0_MSG_PENDING);
	if (err != HAL_OK)
		return err;
	err = HAL_CAN_Start(can->hcan);
	if (err != HAL_OK)
		return err;

	return err;
}

static HAL_StatusTypeDef can_add_filter(can_t *can, uint32_t can_id,
					bool is_extended)
{
	CAN_FilterTypeDef filter;

	if (can->filter_bank >= 14) {
		printf("Max filter banks reached\n");
		return HAL_ERROR; // Max of 14 filter banks per CAN instance
	}

	filter.FilterBank = can->filter_bank++;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterMode = CAN_FILTERMODE_IDLIST;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;

	if (is_extended) {
		/* Extended ID */
		filter.FilterIdHigh = (can_id >> 13);
		filter.FilterIdLow = ((can_id << 3) & 0xFFF8) | (1 << 2);
	} else {
		/* Standard ID */
		filter.FilterIdHigh = (can_id << 5);
		filter.FilterIdLow = 0x0000;
	}

	/* Unused in List Mode */
	filter.FilterMaskIdHigh = 0x0000;
	filter.FilterMaskIdLow = 0x0000;

	filter.FilterActivation = ENABLE;

	return HAL_CAN_ConfigFilter(can->hcan, &filter);
}

HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint32_t can_id_list[])
{
	int size = sizeof(can_id_list) / sizeof(can_id_list[0]);
	for (uint8_t i = 0; i < size; i++) {
		if (can_id_list[i] > 0x7FF) {
			printf("Invalid standard CAN ID at index %d in the CAN ID list. Cannot be larger than 11 bits.\n",
			       i);
			return HAL_ERROR;
		}
		if (can_add_filter(can, can_id_list[i], false) != HAL_OK) {
			printf("can_add_filter failed at index %d\n", i);
			return HAL_ERROR;
		}
	}

	return HAL_OK;
}

HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t can_id_list[])
{
	int size = sizeof(can_id_list) / sizeof(can_id_list[0]);
	for (uint8_t i = 0; i < size; i++) {
		if (can_id_list[i] > 0x1FFFFFFF) {
			printf("Invalid extended CAN ID at index %d in the CAN ID list. Cannot be larger than 29 bits.\n",
			       i);
			return HAL_ERROR;
		}
		if (can_add_filter(can, can_id_list[i], true) != HAL_OK) {
			printf("can_add_filter failed at index %d\n", i);
			return HAL_ERROR;
		}
		if (can_id_list[i] <= 0x7FF) {
			printf("Warning: A CAN ID smaller than 11 bits was added to the extended filter. This was successful, but make sure the id's 'id_is_extended' property is set to true.\n",
			       i);
		}
	}

	return HAL_OK;
}

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
	CAN_TxHeaderTypeDef tx_header;

	if (msg->id_is_extended) {
		tx_header.IDE = CAN_ID_EXT; // Extended CAN ID
		tx_header.ExtId = msg->id;
	} else {
		tx_header.IDE = CAN_ID_STD; // Standard CAN ID
		tx_header.StdId = msg->id;
	}

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
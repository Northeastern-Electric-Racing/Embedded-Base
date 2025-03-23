#include "can.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

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

HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint16_t can_id_list[4])
{
	CAN_FilterTypeDef filter;

	if (can->filter_bank >= 14) {
		printf("Max filter banks reached\n");
		return HAL_ERROR; // Max of 14 filter banks per CAN instance
	}

	/* Check for invalid IDs */
	for (int i = 0; i < 4; i++) {
		if (can_id_list[i] > 0x7FF) {
			printf("CAN ID 0x%X is invalid as a standard ID. Standard IDs cannot be larger than 11 bits.\n",
			       can_id_list[i]);
			return HAL_ERROR;
		}
	}

	filter.FilterBank = can->filter_bank++;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterMode = CAN_FILTERMODE_IDLIST;
	filter.FilterScale = CAN_FILTERSCALE_16BIT;

	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0;
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0;

	filter.FilterIdHigh = (uint16_t)(can_id_list[0] << 5);
	filter.FilterIdLow = (uint16_t)(can_id_list[1] << 5);
	filter.FilterMaskIdHigh = (uint16_t)(can_id_list[2] << 5);
	filter.FilterMaskIdLow = (uint16_t)(can_id_list[3] << 5);

	filter.FilterActivation = ENABLE;

	return HAL_CAN_ConfigFilter(can->hcan, &filter);
}

HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t can_id_list[2])
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

	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0;
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0;

	filter.FilterIdHigh = (can_id_list[0] >> 13);
	filter.FilterIdLow = ((can_id_list[0] << 3) & 0xFFF8) | (1 << 2);
	filter.FilterMaskIdHigh = (can_id_list[1] >> 13);
	filter.FilterMaskIdLow = ((can_id_list[1] << 3) & 0xFFF8) | (1 << 2);

	filter.FilterActivation = ENABLE;

	/* Check for warnings */
	for (int i = 0; i < 2; i++) {
		if (can_id_list[i] <= 0x7FF) {
			printf("Warning: CAN ID 0x%lX is not larger than 11 bits, but was added as an extended ID. This was successful, but make sure 'id_is_extended' is set to true to avoid errors.\n",
			       can_id_list[i]);
		}
	}

	return HAL_CAN_ConfigFilter(can->hcan, &filter);
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
#include "can.h"
#include <stdint.h>
#include <string.h>

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

HAL_StatusTypeDef can_init(can_t *can)
{
	// Set Up Filters
	uint8_t err = HAL_OK;
	uint32_t filters_to_add[4];

	for (uint8_t i = 0; i < can->id_list_len - (can->id_list_len % 4); i++) {
		if ((i + 1) % 4 == 0)
			err = can_add_filter(can, 
								filters_to_add[i-3], 
								filters_to_add[i-2], 
								filters_to_add[i-1], 
								filters_to_add[i]);
		
		filters_to_add[i % 4] = can->id_list[i];
	}

	// Ugly Code
	if (can->id_list_len % 4 == 3)
		err = can_add_filter(can, 
							can->id_list[can->id_list_len-3], 
							can->id_list[can->id_list_len-2], 
							can->id_list[can->id_list_len-1], 
							can->id_list[can->id_list_len-1]);
	else if (can->id_list_len % 4 == 2)
		err = can_add_filter(can, 
							can->id_list[can->id_list_len-2], 
							can->id_list[can->id_list_len-1], 
							can->id_list[can->id_list_len-1], 
							can->id_list[can->id_list_len-1]);
	else if (can->id_list_len % 4 == 1)
		err = can_add_filter(can, 
							can->id_list[can->id_list_len-1], 
							can->id_list[can->id_list_len-1], 
							can->id_list[can->id_list_len-1], 
							can->id_list[can->id_list_len-1]);

	if (err != HAL_OK)
		return err;

	/* set up interrupt & activate CAN */
	HAL_CAN_IRQHandler(can->hcan);

	err = HAL_CAN_ActivateNotification(can->hcan,
					   CAN_IT_RX_FIFO0_MSG_PENDING);
	if (err != HAL_OK)
		return err;
	err = HAL_CAN_Start(can->hcan);
	if (err != HAL_OK)
		return err;

	return err;
}

HAL_StatusTypeDef can_add_filter(can_t *can, uint32_t id1, uint32_t id2, uint32_t id3, uint32_t id4) {
	CAN_FilterTypeDef filter;

	filter.FilterActivation     = ENABLE;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterScale          = CAN_FILTERSCALE_16BIT;
	filter.FilterMode           = CAN_FILTERMODE_IDLIST;
	
	filter.FilterIdLow          = id1 << 5u;
	filter.FilterMaskIdLow      = id2 << 5u;
	filter.FilterIdHigh         = id3 << 5u;
	filter.FilterMaskIdHigh     = id4 << 5u;

	filter.FilterBank      = 0;
	
	uint8_t err = 0;
	err = HAL_CAN_ConfigFilter(can->hcan, &filter);
	if (err != HAL_OK)
		return err;

	return HAL_CAN_Start(can->hcan);
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

HAL_StatusTypeDef can_send_extended_msg(can_t *can, can_msg_t *msg)
{
	CAN_TxHeaderTypeDef tx_header;
	tx_header.StdId = 0;
	tx_header.ExtId = msg->id;
	tx_header.IDE = CAN_ID_EXT;
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

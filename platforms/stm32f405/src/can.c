#include "can.h"
#include <stdint.h>
#include <string.h>

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

HAL_StatusTypeDef can_init(can_t *can) {
	/* set up interrupt & activate CAN */
	HAL_CAN_IRQHandler(can->hcan);

	int8_t err = HAL_CAN_ActivateNotification(can->hcan,
					   CAN_IT_RX_FIFO0_MSG_PENDING);
	if (err != HAL_OK)
		return err;
	err = HAL_CAN_Start(can->hcan);
	if (err != HAL_OK)
		return err;

	return err;
}

HAL_StatusTypeDef can_add_filter(can_t *can, uint32_t id_list[4]) {
	static int filterBank = 0;

	if (filterBank > 7)
		return HAL_ERROR;

	CAN_FilterTypeDef filter;

	filter.FilterActivation     = ENABLE;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterScale          = CAN_FILTERSCALE_16BIT;
	filter.FilterMode           = CAN_FILTERMODE_IDLIST;
	
	filter.FilterIdLow          = id_list[0] << 5u;
	filter.FilterMaskIdLow      = id_list[1] << 5u;
	filter.FilterIdHigh         = id_list[2] << 5u;
	filter.FilterMaskIdHigh     = id_list[3] << 5u;

	filter.FilterBank      		= filterBank;

	filterBank++;

	return HAL_CAN_ConfigFilter(can->hcan, &filter);
} 

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg) {
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

HAL_StatusTypeDef can_send_extended_msg(can_t *can, can_msg_t *msg) {
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

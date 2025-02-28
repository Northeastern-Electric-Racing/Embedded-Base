#include "can.h"
#include <stdint.h>
#include <string.h>

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

HAL_StatusTypeDef can_init(can_t *can)
{
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

HAL_StatusTypeDef can_add_filter(can_t *can, uint32_t id_list[4])
{
	/* Address of filter bank to store filter */
	static int filterBank = 0;

	if (filterBank > 7)
		return HAL_ERROR;

	CAN_FilterTypeDef filter;

	filter.FilterActivation = ENABLE;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterScale = CAN_FILTERSCALE_16BIT;

	/* This filter type makes the filter a whitelist */
	filter.FilterMode = CAN_FILTERMODE_IDLIST;

	/* IDs are shifted left because they are CAN standard IDs */
	filter.FilterIdLow = id_list[0] << 5u;
	filter.FilterMaskIdLow = id_list[1] << 5u;
	filter.FilterIdHigh = id_list[2] << 5u;
	filter.FilterMaskIdHigh = id_list[3] << 5u;

	filter.FilterBank = filterBank;

	filterBank++;

	return HAL_CAN_ConfigFilter(can->hcan, &filter);
}

HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t id_list[2])
{
	/* Address of filter bank to store filter */
	static int filterBank = 0;

	if (filterBank > 7)
		return HAL_ERROR;

	CAN_FilterTypeDef filter;

	filter.FilterActivation = ENABLE;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;

	/* This filter type makes the filter a whitelist */
	filter.FilterMode = CAN_FILTERMODE_IDLIST;

	/* Add the two IDs. They are shifted left by 3 because extended CAN IDs are 29 bits. */
	filter.FilterIdHigh = (id_list[0] & 0x1FFFFFFF) << 3;
	filter.FilterIdLow = (id_list[1] & 0x1FFFFFFF) << 3;

	filter.FilterBank = filterBank;

	filterBank++;

	return HAL_CAN_ConfigFilter(can->hcan, &filter);
}

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
	CAN_TxHeaderTypeDef tx_header;
	(msg->is_extended) ? (tx_header.IDE = CAN_ID_EXT) :
			     (tx_header.IDE = CAN_ID_STD);
	tx_header.StdId = (msg->is_extended) ? 0 : msg->id;
	tx_header.ExtId = (msg->is_extended) ? msg->id : 0;
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
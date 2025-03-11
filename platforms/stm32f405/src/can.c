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

static HAL_StatusTypeDef can_add_filter(CAN_HandleTypeDef *hcan,
					uint32_t can_id, bool is_extended)
{
	CAN_FilterTypeDef filter = { 0 };

	static uint8_t filter_bank = 0;
	if (filter_bank >= 14) {
		return HAL_ERROR;
	}

	filter.FilterBank = filter_bank;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;

	if (is_extended) {
		filter.FilterIdHigh = (can_id >> 13) & 0xFFFF;
		filter.FilterIdLow = (can_id << 3) & 0xFFF8;
		filter.FilterIdLow |= (1 << 2);
	} else {
		filter.FilterIdHigh = (can_id << 5) & 0xFFFF;
		filter.FilterIdLow = 0;
	}

	filter.FilterMaskIdHigh = 0xFFFF;
	filter.FilterMaskIdLow = 0xFFFF;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterActivation = ENABLE;

	HAL_StatusTypeDef status = HAL_CAN_ConfigFilter(hcan, &filter);

	if (status == HAL_OK) {
		filter_bank++;
	}

	return status;
}

HAL_StatusTypeDef can_add_filter_standard(CAN_HandleTypeDef *hcan,
					  uint32_t *can_id_list,
					  uint8_t can_id_list_len)
{
	for (uint8_t i = 0; i < can_id_list_len; i++) {
		if (can_add_filter(hcan, can_id_list[i], false) != HAL_OK) {
			return HAL_ERROR;
		}
	}

	return HAL_OK;
}

HAL_StatusTypeDef can_add_filter_extended(CAN_HandleTypeDef *hcan,
					  uint32_t *can_id_list,
					  uint8_t can_id_list_len)
{
	for (uint8_t i = 0; i < can_id_list_len; i++) {
		if (can_add_filter(hcan, can_id_list[i], true) != HAL_OK) {
			return HAL_ERROR;
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
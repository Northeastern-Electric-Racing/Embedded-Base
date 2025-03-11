#include "can.h"
#include <stdint.h>
#include <string.h>

static int filterBank = 0;

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
	if (filterBank > 7)
		return HAL_ERROR;

	CAN_FilterTypeDef sFilterConfig;

#define REMOTE_FRAME \
	0 /* If = 1 the frame should be a remote frame. If = 0 the frame will be either remote or data frame */
#define EXTID \
	1 /* If = 0 the frame should be a frame with standard ID. If = 1 the frame should be a frame with extended ID */
#define ID	    0x18DAF1DA /* The ID value */
#define MASK	    0x1FFFFFFF /* The mask value */
#define FILTER_ID   ((ID << 3) | (REMOTE_FRAME << 1) | (EXTID << 2))
#define FILTER_MASK ((MASK << 3) | (REMOTE_FRAME << 1) | (EXTID << 2))

	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh =
		(FILTER_ID >> 16); // Filter ID MSB   (FxFR2[0:15])
	sFilterConfig.FilterIdLow =
		(FILTER_ID & 0xFFFF); // Filter ID LSB   (FxFR1[0:15])
	sFilterConfig.FilterMaskIdHigh =
		(FILTER_MASK >> 16); // Filter Mask MSB   (FxFR2[16:31])
	sFilterConfig.FilterMaskIdLow =
		(FILTER_MASK & 0xFFFF); // Filter Mask LSB   (FxFR1[16:31])

	filterBank++;

	return HAL_CAN_ConfigFilter(can->hcan, &sFilterConfig);
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
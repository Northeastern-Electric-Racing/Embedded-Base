// clang-format off
#include "fdcan.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* Initializes CAN */
HAL_StatusTypeDef can_init(can_t *can)
{

	/* Init these guys to 0 */
	can->standard_filter_index = 0;
	can->extended_filter_index = 0;

	/* Config interrupts */
	HAL_StatusTypeDef status = HAL_FDCAN_ConfigInterruptLines(can->hcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0);
	if (status != HAL_OK)
	{
		printf("[fdcan.c/can_init()] ERROR: Failed to run HAL_FDCAN_ConfigInterruptLines() (Status: %d).\n", status);
		return status;
	}

	/* Activate interrupt notifications */
	status = HAL_FDCAN_ActivateNotification(can->hcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	if (status != HAL_OK)
	{
		printf("[fdcan.c/can_init()] ERROR: Failed to run HAL_FDCAN_ActivateNotification() (Status: %d).\n", status);
		return status;
	}

	/* Start FDCAN */
	status = HAL_FDCAN_Start(can->hcan);
	if (status != HAL_OK)
	{
		printf("[fdcan.c/can_init()] ERROR: Failed to run HAL_FDCAN_Start() (Status: %d).\n", status);
		return status;
	}

	return status;
}

/* Adds Standard CAN ID(s) to the CAN filter. */
HAL_StatusTypeDef can_add_filter_standard(can_t *can, uint16_t can_ids[2])
{
	FDCAN_FilterTypeDef filter;

	/* Config the filter */
	filter.IdType = FDCAN_STANDARD_ID;
	filter.FilterIndex = can->standard_filter_index;
	filter.FilterType = FDCAN_FILTER_DUAL;
	filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	filter.FilterID1 = can_ids[0];
	filter.FilterID2 = can_ids[1];

	/* Send HAL the config, and check if it was sucessful */
	HAL_StatusTypeDef status = HAL_FDCAN_ConfigFilter(can->hcan, &filter);
	if (status != HAL_OK)
	{
		printf("[fdcan.c/can_add_filter_standard()] ERROR: Failed to config standard FDCAN filter (Status: %d, Filter Index: %ld).\n", status, filter.FilterIndex);
		return status;
	}

	/* If successful, increment the standard filter index */
	can->standard_filter_index++;
	return status;
}

/* Adds Extended CAN ID(s) to the CAN filter. */
HAL_StatusTypeDef can_add_filter_extended(can_t *can, uint32_t can_ids[2])
{
	FDCAN_FilterTypeDef filter;

	/* Config the filter */
	filter.IdType = FDCAN_EXTENDED_ID;
	filter.FilterIndex = can->extended_filter_index;
	filter.FilterType = FDCAN_FILTER_DUAL;
	filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	filter.FilterID1 = can_ids[0];
	filter.FilterID2 = can_ids[1];

	/* Send HAL the config, and check if it was sucessful */
	HAL_StatusTypeDef status = HAL_FDCAN_ConfigFilter(can->hcan, &filter);
	if (status != HAL_OK)
	{
		printf("[fdcan.c/can_add_filter_extended()] ERROR: Failed to config extended FDCAN filter (Status: %d, Filter Index: %ld). \n", status, filter.FilterIndex);
		return status;
	}

	/* If successful, increment the extended filter index */
	can->extended_filter_index++;
	return status;
}

/* Sends a CAN message. */
HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
	/* Validate message length */
	if (msg->len > 8)
	{
		printf("[fdcan.c/can_send_msg()] ERROR: FDCAN message length exceeds 8 bytes (Length: %d, Message ID: %ld).\n", msg->len, msg->id);
		return HAL_ERROR;
	}

	FDCAN_TxHeaderTypeDef tx_header;
	tx_header.Identifier = msg->id;
	tx_header.DataLength = msg->len;
	tx_header.TxFrameType = FDCAN_DATA_FRAME;
	tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	tx_header.FDFormat = FDCAN_CLASSIC_CAN;

	/* Check if extended or not extended */
	if (msg->id_is_extended)
		tx_header.IdType = FDCAN_EXTENDED_ID;
	else
		tx_header.IdType = FDCAN_STANDARD_ID;

	if (HAL_FDCAN_GetTxFifoFreeLevel(can->hcan) == 0)
		return HAL_BUSY;
	
	HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(can->hcan, &tx_header, msg->data);
	if (status != HAL_OK) {
		printf("[fdcan.c/can_send_msg()] ERROR: HAL_FDCAN_AddMessageToTxFifoQ() failed (Status: %d, Message ID: %ld).\n", status, msg->id);
		return status;
	}

	return status;
}
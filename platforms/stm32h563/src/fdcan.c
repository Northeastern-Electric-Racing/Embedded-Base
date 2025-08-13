#include "fdcan.h"
#include <stdint.h>
#include <string.h>

/* NOTE: STM32H563 will have MAX of 2 CAN buses */
#define MAX_CAN_BUS 2

can_t *can_struct_list[MAX_CAN_BUS] = {NULL, NULL};

static can_callback_t find_callback(FDCAN_HandleTypeDef *hcan)
{
	for (uint8_t i = 0; i < MAX_CAN_BUS; i++)
	{
		if ((can_struct_list[i] != NULL) && (hcan == can_struct_list[i]->hcan))
			return can_struct_list[i]->callback;
	}
	return NULL;
}

/* Add a CAN interfae to be searched for during the event of a callback */
static uint8_t add_interface(can_t *interface)
{
	for (uint8_t i = 0; i < MAX_CAN_BUS; i++)
	{
		/* Interface already added */
		if ((can_struct_list[i] != NULL) && (interface->hcan == can_struct_list[i]->hcan))
			return -1;

		/* If empty, add interface */
		if (can_struct_list[i] == NULL)
		{
			can_struct_list[i] = interface;
			return 0;
		}
	}

	/* No open slots, something is wrong */
	return -2;
}

HAL_StatusTypeDef can_init(can_t *can, can_callback_t callback)
{

	/* Init these guys to 0 */
	can->standard_filter_index = 0;
	can->extended_filter_index = 0;

	/* Store the callback function that's been passed in */
	can->callback = callback;

	/* Start FDCAN */
	HAL_StatusTypeDef status = HAL_FDCAN_Start(can->hcan);
	if (status != HAL_OK)
	{
		printf("[fdcan.c/can_init()] ERROR: Failed to run HAL_FDCAN_Start() (Status: %d).\n", status);
		return status;
	}

	/* Config interrupts */
	status = HAL_FDCAN_ConfigInterruptLines(can->hcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0);
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

	/* Add can_t instance to this file's can_t tracker */
	status = add_interface(can);
	if (status != 0)
	{
		printf("[fdcan.c/can_init()] ERROR: Failed to add a can_t instance to the can_t tracker (Status: %d).\n", status);
		return status;
	}

	return status;
}

/* Callback for any FIFO0 interrupt stuff */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{

	/* If a message has just been recieved... */
	if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
	{
		can_msg_t message;
		FDCAN_RxHeaderTypeDef rx_header;

		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, message.data) == HAL_OK)
		{
			message.id = rx_header.Identifier;
			message.id_is_extended = (rx_header.IdType == FDCAN_EXTENDED_ID);
			message.len = (uint8_t)rx_header.DataLength;

			/* Check size */
			if (rx_header.DataLength > 8)
			{
				printf("[fdcan.c/HAL_FDCAN_RxFifo0Callback()] ERROR: Recieved message is larger than 8 bytes.\n");
				return;
			}

			/* Send message to the application layer via the configured callback */
			can_callback_t callback = find_callback(hfdcan);
			if (callback != NULL)
			{
				callback(&message);
			}
			else
			{
				printf("[fdcan.c/HAL_FDCAN_RxFifo0Callback()] ERROR: callback() is null.\n");
			}
		}
	}
}

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
		printf("[fdcan.c/can_add_filter_standard()] ERROR: Failed to config standard FDCAN filter (Status: %d, Filter Index: %d).\n", status, filter.FilterIndex);
		return status;
	}

	/* If successful, increment the standard filter index */
	can->standard_filter_index++;
	return status;
}

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
		printf("[fdcan.c/can_add_filter_extended()] ERROR: Failed to config extended FDCAN filter (Status: %d, Filter Index: %d). \n", status, filter.FilterIndex);
		return status;
	}

	/* If successful, increment the extended filter index */
	can->extended_filter_index++;
	return status;
}

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
	/* Validate message length */
	if (msg->len > 8)
	{
		printf("[fdcan.c/can_send_msg()] ERROR: FDCAN message length exceeds 8 bytes (Length: %d, Message ID: %d).\n", msg->len, msg->id);
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

	HAL_StatusTypeDef status = HAL_FDCAN_GetTxFifoFreeLevel(can->hcan);
	if (status != HAL_OK)
		printf("[fdcan.c/can_send_msg()] ERROR: HAL_FDCAN_GetTxFifoFreeLevel() failed (Status: %d, Message ID: %d).\n", status, msg->id);
	return status;

	status = HAL_FDCAN_AddMessageToTxFifoQ(can->hcan, &tx_header, msg->data);
	if (status != HAL_OK)
		printf("[fdcan.c/can_send_msg()] ERROR: HAL_FDCAN_AddMessageToTxFifoQ() failed (Status: %d, Message ID: %d).\n", status, msg->id);
	return status;

	return status;
}
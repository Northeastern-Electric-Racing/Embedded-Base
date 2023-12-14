#include "can.h"

// function pointer type for the callback
typedef void (*CAN_Callback)(CAN_HandleTypeDef *hcan);
CAN_Callback myCANCallback;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  // Handle CAN reception event
  if (myCANCallback != NULL)
  {
    myCANCallback(hcan);
  }
}
HAL_StatusTypeDef can_init(can_t *can)
{
    /* set up filter */
    uint16_t high_id = can->id_list[0];
    uint16_t low_id = can->id_list[0];

    for (uint8_t i = 0; i < can->size; i++)
    {
        if (can->id_list[i] > high_id)
        {
            high_id = can->id_list[i];
        }
        if (can->id_list[i] < low_id)
        {
            low_id = can->id_list[i];
        }
    }

    uint32_t full_id = ((uint32_t)high_id << 16) | low_id;

    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterBank = 0;                       // Filter bank number (0 to 27 for most STM32 series)
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;   // Identifier list mode
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  // 32-bit identifier list

    sFilterConfig.FilterIdHigh = (full_id & 0xFFFF0000U) >> 5;
    sFilterConfig.FilterIdLow = (full_id & 0xFFFFU) << 5;

    sFilterConfig.FilterMaskIdHigh = 0xFFFF << 5;       // Set to all ones for ID range
    sFilterConfig.FilterMaskIdLow = 0xFFFF;             // Set to all ones for ID range

    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  // FIFO to assign the filter to
    sFilterConfig.FilterActivation = ENABLE;            // Enable the filter

    uint8_t err = 0;
    err = HAL_CAN_ConfigFilter(&can->hcan, &sFilterConfig);
    if (err != HAL_OK) return err;

    /* set up interrupt & activate CAN */
    err = HAL_CAN_Start(can->hcan);
    if (err != HAL_OK) return err;

    // Override the default callback for CAN_IT_RX_FIFO0_MSG_PENDING
    myCANCallback = can->can_callback;
    err = HAL_CAN_ActivateNotification(can->hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    return err;
}

HAL_StatusTypeDef can_send_msg(can_t *can, can_msg_t *msg)
{
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = msg->id;
    tx_header.ExtId = 0;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = msg->size;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t tx_mailbox;
    HAL_CAN_AddTxMessage(can->hcan, &tx_header, msg->data, &tx_mailbox);

    if (HAL_CAN_GetTxMailboxesFreeLevel(can->hcan) == 0)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}
#include "can.h"
#include <stdint.h>
#include <string.h>

/*
 * NOTE: For implementing callbacks, generate NVIC for selected CAN bus, then
 * implement in `stm32xxxx_it.c`, which STM32CubeMX generates
 */

HAL_StatusTypeDef can_init(can_t *can) {
  /* set up filter */
  uint16_t high_id = can->id_list[0];
  uint16_t low_id = can->id_list[0];

  for (uint8_t i = 0; i < can->id_list_len; i++) {
    if (can->id_list[i] > high_id)
      high_id = can->id_list[i];
    if (can->id_list[i] < low_id)
      low_id = can->id_list[i];
  }

  // uint32_t full_id = ((uint32_t)high_id << 16) | low_id;

  CAN_FilterTypeDef sFilterConfig;

  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  // sFilterConfig.FilterBank = 0;                       /* Filter bank number
  // (0 to 27 for most STM32 series) */ sFilterConfig.FilterMode =
  // CAN_FILTERMODE_IDLIST;   /* Identifier list mode */
  // sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  /* 32-bit identifier
  // list */

  // sFilterConfig.FilterIdHigh = (full_id & 0xFFFF0000U) >> 5;
  // sFilterConfig.FilterIdLow = (full_id & 0xFFFFU) << 5;

  // sFilterConfig.FilterMaskIdHigh = 0xFFFF << 5;       /* Set to all ones for
  // ID range */ sFilterConfig.FilterMaskIdLow = 0xFFFF;             /* Set to
  // all ones for ID range */

  // sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  /* FIFO to assign the
  // filter to */ sFilterConfig.FilterActivation = ENABLE;            /* Enable
  // the filter */

  uint8_t err = 0;
  err = HAL_CAN_ConfigFilter(can->hcan, &sFilterConfig);
  if (err != HAL_OK)
    return err;

  /* set up interrupt & activate CAN */
  HAL_CAN_IRQHandler(can->hcan);

  err = HAL_CAN_ActivateNotification(can->hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  if (err != HAL_OK)
    return err;
  err = HAL_CAN_Start(can->hcan);
  if (err != HAL_OK)
    return err;

  return err;
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

#ifndef MOCK_FUNCTIONS_H
#define MOCK_FUNCTIONS_H

#include "mock_typedefs.h"
#include <stdint.h>

osMutexId_t osMutexNew(const osMutexAttr_t *attr);
uint32_t osMutexAcquire(osMutexId_t mutex, uint32_t timeout);
uint32_t osMutexRelease(osMutexId_t mutex);

osTimerId_t osTimerNew(osTimerFunc_t func, uint32_t type, void *arg, void *attr);
uint32_t osTimerStart(osTimerId_t timer_id, uint32_t ticks);
uint32_t osTimerStop(osTimerId_t timer_id);

void osDelay(uint32_t ms);

osMessageQueueId_t osMessageQueueNew(uint32_t msg_count, uint32_t msg_size, void *attr);
uint32_t osMessageQueuePut(osMessageQueueId_t queue_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
uint32_t osMessageQueueGet(osMessageQueueId_t queue_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
uint32_t osThreadFlagsWait(uint32_t flags, uint32_t options, uint32_t timeout);
uint32_t osThreadFlagsSet (osThreadId_t thread_id, uint32_t flags);
int HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan,
                         uint32_t fifo,
                         CAN_RxHeaderTypeDef *rx_header,
                         uint8_t *rx_data);

uint32_t pdMS_TO_TICKS(uint32_t ms);
HAL_StatusTypeDef HAL_ADC_Start_DMA(void *hadc, uint32_t *pData, uint32_t Length);

HAL_StatusTypeDef HAL_I2C_Mem_Write(void *hi2c, uint16_t devAddress, uint16_t memAddress,
                      uint16_t memAddSize, uint8_t *pData, uint16_t size,
                      uint32_t timeout);

HAL_StatusTypeDef HAL_I2C_Mem_Read(void *hi2c, uint16_t devAddress, uint16_t memAddress,
                     uint16_t memAddSize, uint8_t *pData, uint16_t size,
                     uint32_t timeout);

HAL_StatusTypeDef HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t PinState);

HAL_StatusTypeDef HAL_CAN_GetTxMailboxesFreeLevel(CAN_HandleTypeDef *hcan);

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#define I2C_MEMADD_SIZE_8BIT 1
#define HAL_MAX_DELAY 0xFFFFFFFFU

#define GPIO_PIN_RESET 0
#define GPIO_PIN_SET   1

#define GPIO_PIN_1     1
#define GPIO_PIN_2     2  
#define GPIO_PIN_3     3
#define GPIO_PIN_4     4  
#define GPIO_PIN_5     5
#define GPIO_PIN_6     6
#define GPIO_PIN_7     7
#define GPIO_PIN_8     8
#define GPIO_PIN_9     9
#define GPIO_PIN_10    10
#define GPIO_PIN_11    11
#define GPIO_PIN_12    12
#define GPIO_PIN_13    13
#define GPIO_PIN_14    14
#define GPIO_PIN_15    15
#define GPIO_PIN_16    16

#define CAN_RX_FIFO0 0
#define CAN_ID_EXT 1

#define osFlagsWaitAny 0x00000001U

GPIO_TypeDef portA, portB, portC;
#define GPIOA (&portA)
#define GPIOB (&portB)
#define GPIOC (&portC)

#endif
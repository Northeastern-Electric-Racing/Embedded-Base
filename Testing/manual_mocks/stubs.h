#ifndef MOCK_STUBS_H
#define MOCK_STUBS_H

#include <stdint.h>
#include <math.h>

// clang-format off

#define osWaitForever	0xFFFFFFFFU
#define osTimerPeriodic 0
#define osTimerOnce	1

typedef enum {
	HAL_OK = 0x00U,
	HAL_ERROR = 0x01U,
	HAL_BUSY = 0x02U,
	HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef enum {
	osOK = 0, ///< Operation completed successfully.
	osError =
		-1, ///< Unspecified RTOS error: run-time error but no other error message fits.
	osErrorTimeout =
		-2, ///< Operation not completed within the timeout period.
	osErrorResource = -3, ///< Resource not available.
	osErrorParameter = -4, ///< Parameter error.
	osErrorNoMemory =
		-5, ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
	osErrorISR =
		-6, ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
	osStatusReserved =
		0x7FFFFFFF ///< Prevents enum down-size compiler optimization.
} osStatus_t;

typedef enum {
	osPriorityLow,
	osPriorityBelowNormal,
	osPriorityNormal,
	osPriorityHigh,
	osPriorityRealtime,
	osPriorityRealtime1,
	osPriorityRealtime2,
	osPriorityRealtime3,
	osPriorityRealtime4,
	osPriorityRealtime5,
	osPriorityRealtime6,
	osPriorityRealtime7,
} osPriority_t;

typedef void *ADC_HandleTypeDef;
typedef void *I2C_HandleTypeDef;
typedef void *CAN_HandleTypeDef;
<<<<<<< HEAD
typedef void *FDCAN_HandleTypeDef;
=======
>>>>>>> main
typedef void *SPI_HandleTypeDef;
typedef void *TIM_HandleTypeDef;
typedef void *IWDG_HandleTypeDef;

typedef void *osMutexId_t;
typedef void *osTimerId_t;
typedef void *osThreadId_t;
typedef void *osMessageQueueId_t;
typedef void (*osTimerFunc_t)(void *);

#define osTimerId	 osTimerId_t
#define osMutexId	 osMutexId_t
#define osThreadId	 osThreadId_t
#define osMessageQueueId osMessageQueueId_t

typedef struct {
	const char *name;
	uint32_t stack_size;
	osPriority_t priority;
} osThreadAttr_t;

typedef struct {
	const char *name;
} osTimerAttr_t;

typedef struct {
	const char *name;
} osMutexAttr_t;

typedef struct {
	uint32_t StdId;
	uint32_t ExtId;
	uint32_t IDE;
	uint32_t RTR;
	uint32_t DLC;
} CAN_RxHeaderTypeDef;

typedef struct {
	volatile uint32_t ODR;
} GPIO_TypeDef;

uint32_t HAL_GetTick(void);

<<<<<<< HEAD
=======

>>>>>>> main
osMutexId_t osMutexNew(const osMutexAttr_t *attr);
uint32_t osMutexAcquire(osMutexId_t mutex, uint32_t timeout);
uint32_t osMutexRelease(osMutexId_t mutex);
osTimerId_t osTimerNew(osTimerFunc_t func, uint32_t type, void *arg,
		       void *attr);
uint32_t osTimerStart(osTimerId_t timer_id, uint32_t ticks);
uint32_t osTimerStop(osTimerId_t timer_id);
void osDelay(uint32_t ms);
osMessageQueueId_t osMessageQueueNew(uint32_t msg_count, uint32_t msg_size,
				     void *attr);
uint32_t osMessageQueuePut(osMessageQueueId_t queue_id, const void *msg_ptr,
			   uint8_t msg_prio, uint32_t timeout);
uint32_t osMessageQueueGet(osMessageQueueId_t queue_id, void *msg_ptr,
			   uint8_t *msg_prio, uint32_t timeout);
uint32_t osThreadFlagsWait(uint32_t flags, uint32_t options, uint32_t timeout);
uint32_t osThreadFlagsSet(osThreadId_t thread_id, uint32_t flags);
int HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t fifo,
			 CAN_RxHeaderTypeDef *rx_header, uint8_t *rx_data);
uint32_t pdMS_TO_TICKS(uint32_t ms);
HAL_StatusTypeDef HAL_ADC_Start_DMA(void *hadc, uint32_t *pData,
				    uint32_t Length);

HAL_StatusTypeDef HAL_I2C_Mem_Write(void *hi2c, uint16_t devAddress,
				    uint16_t memAddress, uint16_t memAddSize,
				    uint8_t *pData, uint16_t size,
				    uint32_t timeout);

HAL_StatusTypeDef HAL_I2C_Mem_Read(void *hi2c, uint16_t devAddress,
				   uint16_t memAddress, uint16_t memAddSize,
				   uint8_t *pData, uint16_t size,
				   uint32_t timeout);
HAL_StatusTypeDef HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
				    uint32_t PinState);
HAL_StatusTypeDef HAL_CAN_GetTxMailboxesFreeLevel(CAN_HandleTypeDef *hcan);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint32_t HAL_GetTick(void);
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
					  uint16_t DevAddress, uint8_t *pData,
					  uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
					 uint16_t DevAddress, uint8_t *pData,
					 uint16_t Size, uint32_t Timeout);
int HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
int __HAL_TIM_GET_COUNTER(TIM_HandleTypeDef *htim);
int HAL_Delay(int delay);

#define I2C_MEMADD_SIZE_8BIT 1
#define HAL_MAX_DELAY	     0xFFFFFFFFU

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
#define CAN_ID_EXT   1

#define GPIOA ((GPIO_TypeDef *)0x40020000)
#define GPIOB ((GPIO_TypeDef *)0x40020400)
#define GPIOC ((GPIO_TypeDef *)0x40020800)
#define GPIOD ((GPIO_TypeDef *)0x40020800)
#define GPIOE ((GPIO_TypeDef *)0x40020800)
#define GPIOF ((GPIO_TypeDef *)0x40020800)
#define GPIOG ((GPIO_TypeDef *)0x40020800)

#define osFlagsWaitAny 0x00000001U

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef unsigned long long                      ULONG64;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;
#define ULONG64_DEFINED

typedef struct {
} TX_MUTEX;

typedef struct {
} TX_BYTE_POOL;

typedef struct {
} TX_QUEUE;

// clang-format on

#endif
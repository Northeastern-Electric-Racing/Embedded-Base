#ifndef MOCK_TYPEDEFS_H
#define MOCK_TYPEDEFS_H

#include <stdint.h> 
#include <math.h>

#define osWaitForever 0xFFFFFFFFU
#define osTimerPeriodic 0
#define osTimerOnce 1

typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef enum {
  osOK                      =  0,         ///< Operation completed successfully.
  osError                   = -1,         ///< Unspecified RTOS error: run-time error but no other error message fits.
  osErrorTimeout            = -2,         ///< Operation not completed within the timeout period.
  osErrorResource           = -3,         ///< Resource not available.
  osErrorParameter          = -4,         ///< Parameter error.
  osErrorNoMemory           = -5,         ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
  osErrorISR                = -6,         ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
  osStatusReserved          = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
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

typedef void* ADC_HandleTypeDef;
typedef void* I2C_HandleTypeDef;
typedef void* CAN_HandleTypeDef;
typedef void* SPI_HandleTypeDef;

typedef void* osMutexId_t;
typedef void* osTimerId_t;
typedef void* osThreadId_t;
typedef void* osMessageQueueId_t;
typedef void (*osTimerFunc_t)(void *);

#define osTimerId osTimerId_t
#define osMutexId osMutexId_t
#define osThreadId osThreadId_t
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


#endif
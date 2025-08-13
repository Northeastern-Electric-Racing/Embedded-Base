#ifndef STM32XX_HAL_H
#define STM32XX_HAL_H

#ifdef STM32F405xx
#include "stm32f4xx_hal.h"
#endif

#ifdef STM32H745xx
#include "stm32h7xx_hal.h"
#endif

#ifdef STM32G431xx
#include "stm32g4xx_hal.h"
#endif

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"
#endif

#ifdef STM32H563xx
#include "stm32h5xx_hal.h"
#endif

#endif /* STM32XX_HAL_H*/

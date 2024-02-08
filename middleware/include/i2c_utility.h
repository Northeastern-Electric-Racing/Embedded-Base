#ifndef I2C_UTILITY_H
#define I2C_UTILITY_H

#include "stm32f4xx_hal.h"
#include "timer.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEX_LABELS_H "\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\ta\tb\tc\td\te\tf\n"
#define TAB_SPACE "\t"

static int i2cdetect(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t start, uint8_t end);

#endif // I2C_UTILITY_H
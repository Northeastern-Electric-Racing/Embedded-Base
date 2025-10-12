/**
  ******************************************************************************
  * @file    as3935.h
  * @author  Northeastern Electric Racing
  * @brief   This file contains all the functions prototypes for the
  *          as3935.c driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 Northeastern Electric Racing.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef AS3935_H
#define AS3935_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32h5xx_hal.h"

/* Registers */
#define AS3935_AFE_GB	    0x00
#define AS3935_NF_LEV	    0x01
#define AS3935_WDTH	    0x01
#define AS3935_CL_STAT	    0x02
#define AS3935_MIN_NUM_LIGH 0x02
#define AS3935_SREJ	    0x02
#define AS3935_LCO_FDIV	    0x03
#define AS3935_MASK_DIST    0x03
#define AS3935_INT	    0x03

#define AS3935_S_LIG_MM 0x06 /* Lightning energy register 1 */
#define AS3935_S_LIG_M	0x05 /* Lightning energy register 2 */
#define AS3935_S_LIG_L	0x04 /* Lightning energy register 3 */

#define AS3935_DISTANCE 0x07 /* Lightning distance register */

#define AS3935_DISP_LCO	       0x08
#define AS3935_DISP_SRCO       0x08
#define AS3935_DISP_TRCO       0x08
#define AS3935_TUN_CAP	       0x08
#define AS3935_TRCO_CALIB_DONE 0x3A
#define AS3935_SRCO_CALIB_DONE 0x3B
#define AS3935_PRESET_DEFAULT \
	0x3C /* Register to set all registers in default mode (write 0x96) */
#define AS3935_CALIB_RCO \
	0x3D /* Register to calibrate the internal RC Oscillators (write 0x96) */

#define AS3935_INT_NH 0x01 /* Noise level too high */
#define AS3935_INT_D  0x04 /* Disruptor detected */
#define AS3935_INT_L  0x08 /* Lightning detected */

#define AS3935_AFE_INDOOR  0b10010 /* Indoor mode */
#define AS3935_AFE_OUTDOOR 0b01110 /* Outdoor mode */

#define AS3935_READ_CMD \
	0x4000 /* 0b0100000000000000 First few bits for read command */
#define AS3935_WRITE_CMD \
	0x0 /* 0b0000000000000000 First few bits of write command */

#define AS3935_DISTANCE_OUT_OF_RANGE \
	3F /* 0b111111 This distance reading means storm is out of range */
#define AS3935_DISTANCE_OVERHEAD \
	0x1 /* 0b000001 This distance reading means the storm is overhead */

typedef struct {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
	uint8_t address;
} as3935_t;

void as3935_init(as3935_t *as3935, SPI_HandleTypeDef *hspi,
		 GPIO_TypeDef *cs_port, uint16_t cs_pin);

uint8_t as3935_read(as3935_t *as3935, uint8_t reg);
uint8_t as3935_write(as3935_t *as3935, uint8_t reg, uint8_t value);

uint8_t as3935_reset(as3935_t *as3935);
uint8_t as3935_calibrate_RCO(as3935_t *as3935);

uint8_t as3935_set_AFE(as3935_t *as3935, uint8_t afe_setting);

uint8_t as3935_get_interrupt(as3935_t *as3935);

uint8_t as3935_get_distance(as3935_t *as3935);

uint32_t as3935_get_energy(as3935_t *as3935);

#endif //AS3935_H
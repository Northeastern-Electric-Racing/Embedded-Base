/*
        INA226AQDGSRQ1 Current Sensor I2C Driver
        Datasheet:
        https://www.ti.com/lit/ds/symlink/ina226-q1.pdf
*/

#ifndef INA226_H
#define INA226_H

// REGISTERS
#define INA226_CONFIGURATION 0x00
#define INA226_SHUNT_VOLTAGE 0x01
#define INA226_BUS_VOLTAGE   0x02
#define INA226_POWER	     0x03
#define INA226_CURRENT	     0x04
#define INA226_CALIBRATION   0x05
#define INA226_MASK_ENABLE   0x06
#define INA226_ALERT_LIMIT   0x07
#define INA226_MANUFACTURER  0xFE
#define INA226_DIE_ID	     0xFF

// CONFIGURATION MASKS
#define INA226_CONFIG_RESET_MASK   0x8000 // Bit 15
#define INA226_CONFIG_AVERAGE_MASK 0x0E00 // Bits 9-11
#define INA226_CONFIG_BUSVC_MASK   0x01C0 // Bits 6-8
#define INA226_CONFIG_SHUNTVC_MASK 0x0038 // Bits 3-5
#define INA226_CONFIG_MODE_MASK	   0x0007 // Bits 0-2

// Function Pointers
typedef int (*WritePtr)(uint16_t dev_addr, uint8_t reg, uint16_t *data);
typedef int (*ReadPtr)(uint16_t dev_addr, uint8_t reg, uint16_t *data);

typedef struct {
	uint16_t dev_addr;
	WritePtr write;
	ReadPtr read;
	float current_lsb;
} ina226_t;

/*
    P3T1755DPZ (Temperature Sensor) Driver
    Datasheet: https://www.nxp.com/docs/en/data-sheet/P3T1755.pdf
*/

#ifndef p3t1755_H
#define p3t1755_H
#include <stdint.h>
#include <math.h>

// REGISTERS
// Temperature register: contains two 8-bit data bytes; to store the measured Temp data.
#define p3t1755_TEMPERATURE 0x00 // read only
// Configuration register: contains a single 8-bit data byte; to set the device operating condition
#define p3t1755_CONFIGURATION 0x01
// T_low register: Hysteresis register, it contains two 8-bit data bytes to store the hysteresis T_low limit; default = 75 °C.
#define p3t1755_T_LOW 0x02
// T_high register: Overtemperature shut down threshold register, it contains two 8-bit data bytes to
// store the overtemperature shutdown T_high limit; default = 80 °C.
#define p3t1755_T_HIGH 0x03

// TEMPERATURE REGISTER FORMAT
#define p3t1755_TEMP_RESOLUTION 0.0625f
// temperature range
#define p3t1755_TEMP_MIN -40.0f
#define p3t1755_TEMP_MAX 125.0f

#define p3t1755_RAW_TO_CELSIUS(raw) (((int16_t)(raw)) * p3t1755_TEMP_RESOLUTION)
#define p3t1755_CELSIUS_TO_RAW(temp) \
	((uint16_t)((int16_t)((temp) / p3t1755_TEMP_RESOLUTION)))

// CONFIGURATION MASKS
#define p3t1755_SHUTDOWN_MODE_MASK   0x01 // Bit 0
#define p3t1755_THERMOSTAT_MODE_MASK 0x02 // Bit 1
#define p3t1755_POLARITY_MASK	     0x04 // Bit 2
#define p3t1755_FAULT_QUEUE_MASK     0x18 // Bits 4-3
#define p3t1755_CONVERSION_TIME_MASK 0x60 // Bits 6-5
#define p3t1755_ONE_SHOT_MASK	     0x80 // Bit 7

// FAULT QUEUE SETTINGS
#define p3t1755_1_CONSECUTIVE_FAULT  0x00
#define p3t1755_2_CONSECUTIVE_FAULTS 0x08 // default
#define p3t1755_4_CONSECUTIVE_FAULTS 0x10
#define p3t1755_6_CONSECUTIVE_FAULTS 0x18

// CONVERSION TIME SETTINGS
#define p3t1755_27_5MS_CONVERSION_TIME 0x00
#define p3t1755_55MS_CONVERSION_TIME   0x20 // default
#define p3t1755_110MS_CONVERSION_TIME  0x40
#define p3t1755_220MS_CONVERSION_TIME  0x60

// Function Pointers
typedef int (*WritePtr)(uint16_t dev_addr, uint16_t reg, uint16_t *data);
typedef int (*ReadPtr)(uint16_t dev_addr, uint16_t reg, uint16_t *data);

typedef struct {
	uint16_t dev_addr;
	WritePtr write;
	ReadPtr read;
} p3t1755_t;

void p3t1755_init(p3t1755_t *p3t, WritePtr write, ReadPtr read,
		  uint16_t dev_addr);

int p3t1755_read_reg(p3t1755_t *p3t, uint16_t reg, uint16_t *data);
int p3t1755_write_reg(p3t1755_t *p3t, uint16_t reg, uint16_t *data);

// Reads current temp in celcius
int p3t1755_read_temperature(p3t1755_t *p3t, float *temp_c);

// Config functions
int p3t1755_configure(p3t1755_t *p3t, uint8_t shutdown, uint8_t thermostat,
		      uint8_t polarity, uint8_t fault_queue,
		      uint8_t conversion_time);
int p3t1755_set_shutdown_mode(p3t1755_t *p3t, uint8_t enable);
int p3t1755_set_thermostat_mode(p3t1755_t *p3t, uint8_t enable);
int p3t1755_set_one_shot_mode(p3t1755_t *p3t, uint8_t enable);
int p3t1755_set_polarity(p3t1755_t *p3t, uint8_t setting);
int p3t1755_set_fault_queue(p3t1755_t *p3t, uint8_t data);
int p3t1755_set_conversion_time(p3t1755_t *p3t, uint8_t data);

// Temperature high / low functions
int p3t1755_read_high_temp(p3t1755_t *p3t, float *temp_c);
int p3t1755_read_low_temp(p3t1755_t *p3t, float *temp_c);

int p3t1755_set_high_temp(p3t1755_t *p3t, float temp_c);
int p3t1755_set_low_temp(p3t1755_t *p3t, float temp_c);

#endif
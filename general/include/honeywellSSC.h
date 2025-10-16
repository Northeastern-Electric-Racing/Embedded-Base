/*
    SSCMANN060PG2A3 Air Pressure Sensor I2C Driver
    Datasheet: https://datasheet.octopart.com/SSCMANN060PG2A3-Honeywell-datasheet-36842084.pdf?src-supplier=IHS
    Honeywell I2C Doc: https://prod-edam.honeywell.com/content/dam/honeywell-edam/sps/siot/ja/products/sensors/pressure-sensors/board-mount-pressure-sensors/common/documents/sps-siot-i2c-comms-digital-output-pressure-sensors-tn-008201-3-en-ciid-45841.pdf

    SSCMANN060PG2A3
        Product series - SSC: Standard Accuracy, Compensated/Amplified
        Package - M: SMT (Surface Mount Technology)
        Pressure port - AN: Single axial barbed port
        Options - N: Dry gasses only, no diagnostics
        Pressure range - 060PG: 0 psi to 60 psi
        Output type - 2: I2C, Address 0x28
        Transfer function - A: 10% to 90% of Vsupply (analog), 2^14 counts (digital)
        Supply voltage - 3: 3.3 Vdc

    Pressure Range Specs:
        Pressure Range: 0 to 60 psi
        Over Pressure: 120 psi
        Burst Pressure: 240 psi
        Common Mode Pressure: 250 psi
        Total Error Band: ±2% FFS
        Long-term Stability: ±0.25% FFS
*/

#ifndef honeywellSSC_H
#define honeywellSSC_H
#include <stdint.h>
#include <math.h>

// I2C DEVICE ADDRESS
#define SSC_I2C_ADDR 0x28 // Output type 2

// STATUS BIT VALUES (bits S1 S0 from first byte)
#define SSC_STATUS_NORMAL       0x00 // normal operation, valid data 
#define SSC_STATUS_COMMAND_MODE 0x01 // used for programming, should not be seen in normal operation
#define SSC_STATUS_STALE_DATA   0x02 // data is old
#define SSC_STATUS_DIAGNOSTIC   0x03 // hardware error

// BIT MASKS
#define SSC_STATUS_MASK    0xC000 // Bits 14-15 for 2-bit status in first 2 bytes
#define SSC_PRESSURE_MASK  0x3FFF // Bits 0-13 for 14-bit pressure data in first 2 bytes
#define SSC_TEMP_MASK      0xFFE0 // Bits 6-16 for 11-bit temperature data in last 2 bytes

#define SSC_DATA_LENGTH 4 // first 2 bytes for pressure (and status), 2 bytes for temperature (and junk)

/*
Pressure is calculated from the reading as: 
Pressure = ((output - output_min) * (pressure_max - pressure_min)) / (output_max - output_min) + pressure_min
The following are the constants for that equation
*/
// TRANSFER FUNCTION VALUES (10% to 90% of 2^14 counts) 
#define SSC_OUTPUT_MIN 1638  // 10% of 2^14
#define SSC_OUTPUT_MAX 14746 // 90% of 2^14
#define SSC_COUNTS_FULL_SCALE 16384 // 2^14

// PRESSURE RANGE VALUES (0 to 60 psi)
#define SSC_PRESSURE_MIN 0.0f  // 0 psi
#define SSC_PRESSURE_MAX 60.0f // 60 psi

/*
Temperature is calculated drom the reading as:
Temperature (celcius) = (Output(dec)/2047 * 200) - 50
The following are constants for that equation
*/
// TEMPERATURE CONVERSION CONSTANTS
#define SSC_TEMP_COUNTS_MAX 2047.0f // 11-bit max value
#define SSC_TEMP_OFFSET -50.0f      // Offset in equation
#define SSC_TEMP_SCALE 200.0f       // Scale factor in equation

// Function pointer
typedef int (*ReadPtr)(uint16_t dev_addr, uint8_t *data, uint16_t data_size); //  if data_size == 2: just reads pressure data, if data_size == 4 reads temp data too
// I think the sensor is read-only so no WritePtr needed

// struct that records the current information
typedef struct {
    uint16_t dev_addr;
    ReadPtr read;
    uint8_t min_pressure;
    uint8_t max_pressure;
} honeywellSSC_t;

// sets stuff up
void honeywellSSC_init(honeywellSSC_t *ssc, ReadPtr read, uint16_t dev_addr, uint8_t min_pressure, uint8_t max_pressure);

// Reads the data into the provided data pointer
// expects honeywellSSC_t and a pointer to a uint8_t array of length 2 or 4
int honeywellSSC_read_data(honeywellSSC_t *ssc, uint8_t *data, uint8_t data_size);

// Reads the status message
void honeywellSSC_read_status(uint16_t data, uint8_t *result);

// Reads the pressure in psi
int honeywellSSC_read_pressure(honeywellSSC_t *ssc, float *result);

// Reads the temperature in celcius
int honeywellSSC_read_temp(honeywellSSC_t *ssc, float *result);

#endif
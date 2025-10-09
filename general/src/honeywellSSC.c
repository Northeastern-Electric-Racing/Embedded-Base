/*
    SSCMANN060PG2A3 Air Pressure Sensor I2C Driver
    Datasheet: https://datasheet.octopart.com/SSCMANN060PG2A3-Honeywell-datasheet-36842084.pdf?src-supplier=IHS
    Honeywell I2C Doc: https://prod-edam.honeywell.com/content/dam/honeywell-edam/sps/siot/ja/products/sensors/pressure-sensors/board-mount-pressure-sensors/common/documents/sps-siot-i2c-comms-digital-output-pressure-sensors-tn-008201-3-en-ciid-45841.pdf
*/

#include "honeywellSSC.h"

// sets stuff up
void honeywellSSC_init(honeywellSSC_t *ssc, ReadPtr read, uint16_t dev_addr) {
    ssc->read = read;
    ssc->dev_addr = dev_addr;
}

// Reads the data into the provided data pointer
// expects honeywellSSC_t and a pointer to a uint8_t array of length 2 or 4
int honeywellSSC_read_data(honeywellSSC_t *ssc, uint8_t *data, uint8_t data_size) {
    if (data_size != 2 && data_size != 4) {
        return -1;
    }
    return ssc->read(ssc->dev_addr, data, data_size);
}

// Reads the status message
void honeywellSSC_read_status(uint16_t data, uint8_t *result) {
    uint16_t status = (data & SSC_STATUS_MASK) >> 14;
    *result = (uint8_t)status;
}

// Reads the pressure in psi
int honeywellSSC_read_pressure(honeywellSSC_t *ssc, float *result) {
    uint8_t data_array[2];

    int success_msg = honeywellSSC_read_data(ssc, data_array, 2);
    if (success_msg != 0) {
        return success_msg;
    }

    uint16_t status_and_pressure_data = ((uint16_t)data_array[0] << 8) | data_array[1];

    uint8_t status;
    honeywellSSC_read_status(status_and_pressure_data, &status);
    if (status != SSC_STATUS_NORMAL) {
        return -1;
    }

    uint16_t pressure_data = status_and_pressure_data & SSC_PRESSURE_MASK;
    // Pressure = ((output - output_min) * (pressure_max - pressure_min)) / (output_max - output_min) + pressure_min
    float pressure_reading = (((pressure_data - SSC_OUTPUT_MIN) * (SSC_PRESSURE_MAX - SSC_PRESSURE_MIN)) / (SSC_OUTPUT_MAX - SSC_OUTPUT_MIN) + SSC_PRESSURE_MIN);

    *result = pressure_reading;
    return 0;
}

// Reads the temperature in celcius
int honeywellSSC_read_temp(honeywellSSC_t *ssc, float *result) {
    uint8_t data_array[4];

    int success_msg = honeywellSSC_read_data(ssc, data_array, 4);
    if (success_msg != 0) {
        return success_msg;
    }

    uint16_t status_and_pressure_data = ((uint16_t)data_array[0] << 8) | data_array[1];
    uint16_t temp_raw = ((uint16_t)data_array[2] << 8) | data_array[3];
    uint16_t temp_data = (temp_raw & SSC_TEMP_MASK) >> 5;

    uint8_t status;
    honeywellSSC_read_status(status_and_pressure_data, &status);
    if (status != SSC_STATUS_NORMAL) {
        return -1;
    }

    // Temperature (celcius) = (Output(dec)/2047 * 200) - 50
    float temperature_reading = ((float)temp_data / SSC_TEMP_COUNTS_MAX) * SSC_TEMP_SCALE + SSC_TEMP_OFFSET;

    *result = temperature_reading;
    return 0;
}
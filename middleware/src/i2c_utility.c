#include "i2c_utility.h"
#define MAX_TRIALS 3    // Max tries to probe
#define MAX_TIMEOUT 50  // units in ms
#define HEX 16

/**
 * TODO: Make sure the buffer is the right size.
 * TODO: Implement modes for I2C Communication.
 * TODO: Implement for flags.
 */
static int i2cdetect(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t start, uint8_t end) {
    // Initialize the buffer and local variables
    HAL_StatusTypeDef ret;
    uint8_t row = 1;
    char status[sizeof(uint8_t) * 8 + 1];
    char *labels[] = {"0x0", "0x1", "0x2", "0x3", "0x4", "0x5", "0x6", "0x7"};

    // Add to the appropriate buffer
    buffer[0] = HEX_LABELS_H; //add labels to the first row of the buffer

    // Loop through each device address from the start to end
    for(int i = 0x00U; i <= 0x70U; i+=0x10U) {
        buffer[row] = labels[row - 1];
        for(int j = 0; j < 16; j++) {
            uint8_t devAddr = i + j;
            // out of range reading
            if(devAddr < start) {
                status = SPACING;
            }
            else if (devAddr > end) {
                status = SPACING;
            }
            // 
            else {
                // Use HAL_I2C_IsDeviceReady
                ret = HAL_I2C_IsDeviceReady(hi2c, ((uint16_t) (devAddr << 1)), MAX_TRIALS, MAX_TIMEOUT); 
        
                // Device status case
                switch (ret) {
                    case HAL_BUSY:
                        status = "UU"; // the bus is considered busy
                        break;
                    case HAL_OK:
                        utoa(devAddr, status, HEX); // reads the hexadecimal address and turns it into a string
                        break;
                    case HAL_TIMEOUT:
                        status = "--"; // no response from device
                        break;
                    case HAL_ERROR:
                    default:
                        return HAL_ERROR; // ask for help on error handling.
                        break;
                }
            }

            // Add status to the buffer
            strcat(buffer[row], SPACING); // spacing for string
            strcat(buffer[row], status); // actual status

            // clear char array
            memset(status, 0, strlen(status));
        }
        // table update
        strcat(buffer[row], "\n");
        row++;
    }

    // Return normal status
    return 0;
}

/**
 * TODO: Implementation of i2cdump using HAL_I2C_Master_Receive
 * HAL_I2C_Master_Receive() - requests data from slave device.
 * HAL_I2C_Mem_Read() - requests data from slave device from a specific memory address.
 */
static int i2cdump(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t offset, uint8_t start, uint8_t end) {
    

    // nominal return
    return HAL_OK;
}
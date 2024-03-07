#include "i2c_utility.h"
#define MAX_TRIALS 3    // Max tries to probe
#define HEX 16
char *hex_labels[] = {"00:", "10:", "20:", "30:", "40:", "50:", 
    "60:", "70:", "80:", "90:", "a0:", "b0:", "c0:", "d0:", "e0:", "f0:"};
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

    // Add to the appropriate buffer
    buffer[0] = HEX_LABELS_H; //add labels to the first row of the buffer

    // Loop through each device address from the start to end
    for(int i = 0x00U; i <= 0x70U; i+=0x10U) {
        buffer[row] = hex_labels[row - 1];
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
                ret = HAL_I2C_IsDeviceReady(hi2c, ((uint16_t) (devAddr << 1)), MAX_TRIALS, HAL_MAX_DELAY); 
        
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
 * HAL_I2C_Master_Receive() - requests data from slave device. (BLOCKING)
 * HAL_I2C_Mem_Read() - requests data from slave device from a specific memory address. (NON-BLOCKING)
 */
static int i2cdump(I2C_HandleTypeDef *hi2c, uint16_t devAddress, char **buffer, char mode, uint8_t start, uint8_t end) {
    // Prepare the buffer
    int row = 0;

    // Add the labels to the first row
    buffer[row] = HEX_LABELS_H;
    row ++;

    // need to read from the given address of a I2C Bus.
    switch(mode) {
        case 'w': // A word (4 bytes or 16-bit)
            break;
        case 's': // A SMBus Block
            break;
        case 'i': // I2C Block
            break;
        case 'b': // Byte sized (default)
        default:
            uint8_t data;
            for (int i = 0x00U; i <= 0xf0U; i += 0x10U) {
                buffer[row] = hex_labels[row - 1];
                char data_str[sizeof(char) * 4 + 1];
                for(int j = 0x00U; j <= 0x0fU; j++) {
                    uint16_t reg = i + j; // get the memory address
                    // read memory address
                    if (HAL_I2C_Mem_Read(hi2c, (devAddress << 1), reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY) != HAL_OK) {
                        // error
                        return HAL_ERROR;
                    }
                    
                    // Convert data buffer into the char buffer
                    utoa(data, data_str, HEX);
                    
                    // display the value from the memory address
                    strcat(buffer[row], SPACING);
                    strcat(buffer[row], data_str);

                    // reset the string array
                    memset(data_str, 0, strlen(data_str));
                }
                row++;
            }
            break;
    }
    // nominal return
    return HAL_OK;
}
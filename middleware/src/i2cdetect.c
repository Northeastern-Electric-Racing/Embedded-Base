#include "i2cdetect.h"
#define MAX_TRIALS 3    // Max tries to probe
#define MAX_TIMEOUT 50  // units in ms
#define HEX 16          

static int i2cdetect(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t start, uint8_t end) {
    // Initialize the buffer and local variables
    HAL_StatusTypeDef ret;
    uint8_t i;
    uint8_t row = 1;
    char status[sizeof(uint8_t) * 8 + 1];
    char *labels[] = {"0x0      ", "0x1", "0x2", "0x3", "0x4", "0x5", "0x6", "0x7"};

    // Add to the appropriate buffer
    buffer[0] = HEX_LABELS_H; //add labels to the first row of the buffer

    // Add labels to buffer
    for(int j = 1; j <= 7; j++) {
        strcat(buffer[j], labels[j-1]);
    }
    
    // Loop through each device address from the start to end
    for(i = start; i <= end; i++) {
        row = ((row * HEX)<= i) ? row + 1 : row;
        // Use HAL_I2C_IsDeviceReady
        ret = HAL_I2C_IsDeviceReady(hi2c, (i << 1), MAX_TRIALS, MAX_TIMEOUT); 
        
        // Device status case
        switch (ret) {
            case HAL_BUSY:
                status = "UU";
                break;
            case HAL_OK:
                utoa(i, status, HEX);
                break;
            case HAL_TIMEOUT:
                status = "--";
                break;
            case HAL_ERROR:
            default:
                return HAL_ERROR; // ask for help on error handling.
                break;
        }

        // Add status to the buffer
        strcat(buffer[row], SPACING); // spacing for string
        strcat(buffer[row], status); // actual status
    }

    // Return normal status
    return HAL_OK;
}

int main(int argc, char *argv[]) {
    I2C_HandleTypeDef *hi2c;
    int flags = 0;
    int confirm = 0, version = 0, list = 0;
    uint8_t first = 0x03U, last = 0x77U; 
    // Handle any flags
    while(argc > flags + 1 && argv[flags + 1][0] == "-") {
        switch(argv[flags+1][1]) {
            case "F": // display functionality
                
                break;
            case "l": // list all i2c busses
                
                break;
            case "y": // automatic run
                confirm = 1;
                break;
            case "a":
                first = 0x00U;
                last = 0x7F;
                break;
            default:
                break;
        }
    }
}


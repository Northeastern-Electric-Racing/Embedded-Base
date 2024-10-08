#include "i2c_utility.h"

char *hex_labels[] = {"00:", "10:", "20:", "30:", "40:", "50:", "60:", "70:",
                      "80:", "90:", "a0:", "b0:", "c0:", "d0:", "e0:", "f0:"};

/**
 * TODO: Implement modes for I2C Communication.
 * TODO: Implement for flags.
 * TODO: Check for error handling
 */
int i2cdetect(I2C_HandleTypeDef *hi2c, char **buffer, int mode, uint8_t start,
              uint8_t end) {
  // Initialize the buffer and local variables
  HAL_StatusTypeDef ret;
  uint8_t row = 1;
  char status[sizeof(uint8_t) * 8 + 1];

  // Add to the appropriate buffer
  buffer[0] = HEX_LABELS; // add labels to the first row of the buffer

  // Loop through each device address from the start to end
  for (unsigned int i = 0x00U; i <= 0x70U; i += 0x10U) {
    strcat(buffer[row], hex_labels[row - 1]);
    for (unsigned int j = 0x00U; j < 0x10U; j += 0x01U) {
      uint8_t devAddr = i + j;
      // out of range reading
      if (devAddr < start || devAddr > end) {
        strcpy(status, SPACING);
      }
      // in range
      else {
        // Use HAL_I2C_IsDeviceReady
        ret = HAL_I2C_IsDeviceReady(hi2c, (devAddr << 1), MAX_TRIALS, TIMEOUT);

        // Device status case
        switch (ret) {
        case HAL_BUSY:
          strcpy(status, "UU"); // the bus is considered busy
          break;
        case HAL_OK:
          utoa(devAddr, status,
               HEX); // reads the hexadecimal address and turns it into a string
          break;
        case HAL_ERROR:
        case HAL_TIMEOUT:
        default:
          strcpy(status, "--"); // no response from device or not found
          break;
        }
      }

      // Add status to the buffer
      strcat(buffer[row], SPACING); // spacing for string
      strcat(buffer[row], status);  // actual status

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
 * TODO: Implement different reading modes
 * HAL_I2C_Master_Receive() - requests data from slave device. (BLOCKING)
 * HAL_I2C_Mem_Read() - requests data from slave device from a specific memory
 * address. (NON-BLOCKING)
 */
int i2cdump(I2C_HandleTypeDef *hi2c, uint16_t devAddress, char **buffer,
            char mode, uint8_t start, uint8_t end) {
  // Prepare the buffer
  int row = 0;

  // need to read from the given address of a I2C Bus.
  switch (mode) {
  case 'w': // A word (4 bytes or 32-bit)
    buffer[row] = "\t\t0    4    8    b";
    row++;

    uint8_t data1 = 0;
    for (unsigned int i = 0x00U; i <= 0xf0U; i += 0x10U) {
      buffer[row] = hex_labels[row - 1];
      char data_str[sizeof(char) * 4 + 1];

      for (unsigned int j = 0x00U; j <= 0x0fU; j += 0x04U) {
        uint16_t reg = i + j;
        // read memory address
        if (HAL_I2C_Mem_Read(hi2c, (devAddress << 1), reg, I2C_MEMADD_SIZE_8BIT,
                             &data1, 4, HAL_MAX_DELAY) != HAL_OK) {
          // error
          return HAL_ERROR;
        }

        // convert data into char text
        utoa(data1, data_str, HEX);

        // display the value from the memory address
        strcat(buffer[row], SPACING);
        strcat(buffer[row], data_str);

        // reset the string array
        memset(data_str, 0, strlen(data_str));
      }
      strcat(buffer[row], "\n");
      row++;
    }
    break;
  case 's': // A SMBus Block
    break;
  case 'i': // I2C Block
    break;
  case 'b': // Byte sized (default)
  default:
    // Add the labels to the first row
    buffer[row] = HEX_LABELS;
    row++;

    uint8_t data = 0;
    for (unsigned int i = 0x00U; i <= 0xf0U; i += 0x10U) {
      // add the vertical labels
      buffer[row] = hex_labels[row - 1];
      char data_str[sizeof(char) * 2 + 1];

      for (unsigned int j = 0x00U; j <= 0x0fU; j++) {
        uint16_t reg = i + j; // get the memory address
        // read memory address
        if (HAL_I2C_Mem_Read(hi2c, (devAddress << 1), reg, I2C_MEMADD_SIZE_8BIT,
                             &data, 1, HAL_MAX_DELAY) != HAL_OK) {
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
      strcat(buffer[row], "\n");
      row++;
    }
    break;
  }
  // nominal return
  return HAL_OK;
}

/**
 * @warning serial_print max size is 128 bytes.
 */
void printResult(char **buffer, int len) {
  for (int i = 0; i < len; i++) {
    serial_print(buffer[i]);
  }
}
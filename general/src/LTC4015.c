#include "LTC4015.h"



HAL_StatusTypeDef LTC4015_Init(LTC4015_T *dev, I2C_HandleTypeDef *i2cHandle)
{
  dev -> i2cHandle = i2cHandle;
  
  //Gets the value from the Charging state register 
  LtC4015_read(LTC4015_T *dev, LTC4015_CHGSTATE, dev -> CHGdata) // not sure if this is wanted in the init function
  uint8_t data[10];
  
  //Storing charging fault data seperatly, mostly to just show what each bit actually represents
  Data[0] = (dev -> CHGdata)&1;		  // Battery short
  Data[1] = (dev -> CHGdata)&2;	  	// Battery missing
  Data[2] = (dev -> CHGdata)&4;     // Max Charge Time (Don't need?)
  Data[3] = (dev -> CHGdata)&8;		  // C/x termination
  Data[4] = (dev -> CHGdata)&16;	  // Timer termination (Dont need?)
  Data[5] = (dev -> CHGdata)&32;	  // Thermistor pause state (Dont need?)
  Data[6] = (dev -> CHGdata)&64;	  // CC/CV
  Data[7] = (dev -> CHGdata)&128;   // pre-charge
  Data[8] = (dev -> CHGdata)&256;   // charger suspended
  Data[9] = (dev -> CHGdata)&512;	  // absorb
  Data[10] = (dev -> CHGdata)&1024; // Lead-acid equalization charge state (Don't need?)

  

}


HAL_StatusTypeDef LTC4015_read(LTC4015_T *dev, uint16_t reg, uint16_t *data)
{
  return HAL_I2C_Mem_Read(dev->i2c_handle, LTC4015_ADDR_68, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
  
}

HAL_StatusTypeDef LTC4015_write(LTC4015_T *dev, uint16_t reg, uint16_t data)
{
  return HAL_I2C_Mem_Write(dev->i2c_handle, LTC4015_ADDR_68, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

//add alerts from the board, also make customisable alarts. get columb counter information, tempurater infomration
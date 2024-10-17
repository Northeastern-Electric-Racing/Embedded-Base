#include "pca9539.h"

#include "../../middleware/include/c_utils.h"

#define REG_SIZE_BITS 8


void pca9539_init(pca9539_t *pca, WritePtr writeFunc,
		  ReadPtr readFunc, uint8_t dev_addr)
{
	pca->dev_addr = dev_addr << 1u;
}


int pca9539_read_reg(pca9539_t *pca, uint16_t reg_type, uint8_t *buf)
{
	return pca->read(pca->dev_addr, reg_type, *buf);
}


int pca9539_write_reg(pca9539_t *pca, uint16_t reg_type, uint8_t buf)
{
	return pca->write(pca->dev_addr, reg_type, buf);
}

//Errors in write_pin and read_pin functions if the commented code left in
//Variables within function are unused
int pca9539_write_pin(pca9539_t *pca, uint16_t reg_type, uint8_t pin,
		      uint8_t buf)
{
	//uint8_t data;
	//uint8_t data_new; 
	

	int status = pca->write(pca->dev_addr, reg_type, buf);
	if (status) {
		return status;
	}

	//data_new = (data & ~(1u << pin)) | (buf << pin);

	return pca->write(pca->dev_addr, reg_type, buf);
}


int pca9539_read_pin(pca9539_t *pca, uint16_t reg_type, uint8_t pin,
		     uint8_t *buf)
{
	//uint8_t data;

	int status = pca->read(pca->dev_addr, reg_type, *buf);
	if (status) {
		return status;
	}
	//*buf = (data & (1 << pin)) > 0; 
	
	return pca->read(pca->dev_addr, reg_type, *buf);
}

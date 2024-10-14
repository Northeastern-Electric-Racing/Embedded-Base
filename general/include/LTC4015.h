/*
	LTC4015EUHF#PBF Load Switch Expander Source File
	Link to part Datasheet for reference:
	https://www.diodes.com/assets/Datasheets/PI4IOE5V9535.pdf

	Author: David Noble
*/

#ifndef LTC4015_H_
#define LTC4015_H_

#include <stdint.h>
#include "stm32xx_hal.h"

//Device address
#define LTC4015_ADDR_68 (0x68)

//Enable Register for Specified alert register, page 49-50
#define EN_LIMIT_ALERTS		0x0D //LIMIT_ALERTS
#define EN_CHARGER_STATE_ALERTS 0x0E //CHARGER_STATUS_ALERTS
#define EN_CHARGE_STATUS_ALERTS 0x0F //CHARGE_STATUS_ALERTS

//Register storing if specifed fault has occurred, each bit represents different fault, page 52-53
#define LTC4015_CHGSTATE 0x34 //Real time battery charger state indicator
#define CHARGE_STATUS	 0x35

//Errors need to be enabled, with remain high once flagged till reset or disabled
#define LIMIT_ALERTS	     0x36
#define CHARGER_STATE_ALERTS 0x37
#define CHARGE_STATUS_ALERTS 0x38
#define SYSTEM_STATUS	     0x39

//Coulomb Counter
#define QCOUNT_LO_ALERT_LIMIT  0x10
#define QCOUNT_HI_ALERT_LIMIT  0x11
#define QCOUNT_PRESCALE_FACTOR 0x12
#define QCOUNT		       0x13
#define CONFIG_BITS	       0x14
//Enable Coulomb Counter Low Alert.(0xOD Bit 13)
//Enable Coulomb Counter High Alert.(0xOD Bit 12)

typedef int (*Read_Ptr)(uint16_t DevAddress, uint16_t MemAddress,
			uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

typedef int (*Write_Ptr)(uint16_t DevAddress, uint16_t MemAddress,
			uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

typedef struct {
	uint16_t chargeFaults; //Stores error faults from CHGSTATE register
	uint16_t qcount;
	uint16_t limitAlerts;

	Read_Ptr read;
	Write_Ptr write;
} LTC4015_T;

/**
 * @brief Initializes the LTC4015EUHF#PBF
 *
 * @param dev
 * @param hi2c
 * @return HAL_StatusTypeDef
 */
int LTC4015_Init(LTC4015_T *dev, Read_Ptr read, Write_Ptr write);

/**
 * @brief Reads from the LTC4015EUHF#PBF load switch 
 * @note always reads from both input registers
 *
 * @param dev
 * @param i2c_handle
 */
int LTC4015_read(LTC4015_T *dev, uint16_t reg, uint16_t *data);

/**
 * @brief Writes to the LTC4015EUHF#PBF load switch 
 *
 * @param device
 * @param data
 * @param i2c_handle
 * @return HAL_StatusTypeDef
 */
int LTC4015_write(LTC4015_T *dev, uint16_t reg, uint16_t data);

/**
 * @brief Checks coulombs of charger to see if within a given range, more info on page 38-40
 *
 * @param dev
 * @param prescaler
 * @param highAlert
 * @param prescaler
 * @param lowAlert
 * @return HAL_StatusTypeDef, QCOUNT, Faulted 
 */
int LTC4015_Qcounter(LTC4015_T *dev, uint16_t prescaler, uint16_t highAlert,
		     uint16_t lowAlert);

#ifndef MAX_NUM_LTC4015_INSTANCES
#define MAX_NUM_LTC4015_INSTANCES 1
#endif

#endif

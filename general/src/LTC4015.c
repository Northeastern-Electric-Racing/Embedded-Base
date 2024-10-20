/*
        LTC4015EUHF#PBF Load Switch Expander Source File
        Link to part Datasheet for reference:
        https://www.diodes.com/assets/Datasheets/PI4IOE5V9535.pdf

        Author: David Noble
*/

#include "LTC4015.h"

int LTC4015_Init(LTC4015_T *dev, Read_Ptr read, Write_Ptr write)
{
	dev->read = read;
	dev->write = write;

	//Gets the value from the Charging state register
	LtC4015_read(dev, LTC4015_CHGSTATE, &dev->chargeFaults);
}

int LTC4015_read(LTC4015_T *dev, uint16_t reg, uint16_t *data)
{
	return dev->read(LTC4015_ADDR_68, reg, 0x00000001U, data, 1);
}

int LTC4015_write(LTC4015_T *dev, uint16_t reg, uint16_t data)
{
	return dev->write(LTC4015_ADDR_68, reg, 0x00000001U, data, 1);
}

int LTC4015_Qcounter(LTC4015_T *dev, uint16_t prescaler, uint16_t highAlert,
		     uint16_t lowAlert)
{
	//Increases integration time, at which QCOUNT is updated
	LTC4015_write(dev, QCOUNT_PRESCALE_FACTOR, prescaler);

	//Sets the High amount for the QCOUNT before error is flagged
	LTC4015_write(dev, QCOUNT_HI_ALERT_LIMIT, highAlert);
	LTC4015_write(dev, EN_LIMIT_ALERTS,
		      0x1000); //Enable bit is in 12th bit postion

	//Sets the low amount for the QCOUNT before error is flagged
	LTC4015_write(dev, QCOUNT_LO_ALERT_LIMIT, lowAlert);
	LTC4015_write(dev, EN_LIMIT_ALERTS,
		      0x2000); //Enable bit is in 13th bit postion

	LTC4015_write(dev, CONFIG_BITS,
		      0x0008); //enable QCOUNT, in bit postion 2
	LTC4015_read(dev, LIMIT_ALERTS, dev->limitAlerts);
	LTC4015_read(dev, QCOUNT, dev->qcount);

	//This all could be put into a while loop if you want to continually check for errors
	LTC4015_write(dev, CONFIG_BITS,
		      (CONFIG_BITS ^
		       0x1000)); //should re-enable charging if was disabled
	//Sees if the alerts are being flagged, and then will return the QCOUNT
	if (LIMIT_ALERTS | 0x1000 == 0x1000) {
		LTC4015_write(
			dev, EN_LIMIT_ALERTS,
			(EN_LIMIT_ALERTS ^
			 0x1000)); //Should just reset the enable but touch nothing else
		LTC4015_write(dev, CONFIG_BITS, 0x1000); //suspends charger
		return (QCOUNT,
			highAlert); //Need away to tell its being flagged, but not really sure what to return
	} else if (LIMIT_ALERTS | 0x2000 == 0x2000) {
		LTC4015_write(dev, EN_LIMIT_ALERTS, EN_LIMIT_ALERTS ^ 0x2000);
		LTC4015_write(dev, CONFIG_BITS, 0x1000); //suspends charger
		return (QCOUNT, lowAlert); //sames issue as above
	}
}
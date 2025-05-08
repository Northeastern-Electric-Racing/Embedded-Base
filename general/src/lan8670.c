#include "lan8670.h"

/**
 * @brief Modifies a specific bit in a register of the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param reg The register address to modify.
 * @param bit_mask The bit mask to modify.
 * @param setting true to set the bit, false to clear it.
 * @return 0 on success, or a non-zero error code.
 */
static int modify_register_bit(lan8670_t *lan, int reg, uint32_t bit_mask,
			       bool setting)
{
	uint32_t data = 0;
	int status = lan->read(lan->device_address, reg, &data);
	if (status != 0)
		return status; // ERROR: Read failed.

	if (setting) {
		data |= bit_mask; // Set the bit specified by bit_mask
	} else {
		data &= ~bit_mask; // Clear the bit specified by bit_mask
	}

	return lan->write(lan->device_address, reg, &data);
}

/**
 * @brief Reads a register from the LAN8670's MMD interface.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param data Buffer to store the read data.
 * @return 0 on success, or a non-zero error code.
 */
static int read_mmd_register(lan8670_t *lan, uint16_t mmd_addr,
			     uint16_t register_offset, uint16_t *data)
{
	/* Set DEVAD field and FNCTN = 00 (Address) */
	uint16_t mmd_ctrl =
		mmd_addr &
		0x1F; /* DEVAD in bits 4:0, FNCTN in bits 15:14 = 00 */
	int status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0)
		return status;

	/* Write register offset to MMDAD */
	status = lan->write(lan->device_address, REG_MMDAD, register_offset);
	if (status != 0)
		return status;

	/* Set DEVAD and FNCTN = 01 (Data, no post increment) */
	mmd_ctrl = (mmd_addr & 0x1F) | (1 << 14); /* Set FNCTN = 01 */
	status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0)
		return status;

	/* Read data from MMDAD */
	return lan->read(lan->device_address, REG_MMDAD, data);
}

/**
 * @brief Writes to a register in the LAN8670's MMD interface.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param data The data to be written to the register.
 * @return 0 on success, or a non-zero error code.
 */
static int write_mmd_register(lan8670_t *lan, uint16_t mmd_addr,
			      uint16_t register_offset, uint16_t data)
{
	/* Set DEVAD field and FNCTN = 00 (Address) */
	uint16_t mmd_ctrl =
		mmd_addr &
		0x1F; /* DEVAD in bits 4:0, FNCTN in bits 15:14 = 00 */
	int status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0)
		return status;

	/* Write register offset to MMDAD */
	status = lan->write(lan->device_address, REG_MMDAD, register_offset);
	if (status != 0)
		return status;

	/* Set DEVAD and FNCTN = 01 (Data, no post increment) */
	mmd_ctrl = (mmd_addr & 0x1F) | (1 << 14); /* Set FNCTN = 01 */
	status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0)
		return status;

	/* Write data to MMDAD */
	return lan->write(lan->device_address, REG_MMDAD, data);
}

/**
 * @brief Initializes a LAN8670 instance.
 * @param lan Pointer to the lan8670_t instance.
 * @param device_address The address of the LAN8670.
 * @param read Function pointer for reading data from the LAN8670.
 * @param write Function pointer for writing data to the LAN8670.
 */
void lan8670_init(lan8670_t *lan, uint32_t device_address, ReadFunction read,
		  WriteFunction write)
{
	lan->write = write;
	lan->read = read;
	lan->device_address = device_address;
}

/**
 * @brief Performs a software reset of the LAN8670 Ethernet PHY.
 * @param lan Pointer to the lan8670_t instance.
 * @return 0 on success, or a non-zero error code.
 */
int lan8670_reset(lan8670_t *lan)
{
	uint32_t data =
		0x8000; // Makes bit 15 = 1. This starts a software reset of the PHY.
	return lan->write(lan->device_address, REG_BASIC_CONTROL, &data);
}

/**
 * @brief Enables or disables loopback mode on the LAN8670.
 *
 *  When enabled, transmit data (TXD) pins from the
 *  MAC will be looped back onto the receive data (RXD) pins to the MAC. In this mode, no signal is
 *  transmitted onto the network media
 *
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable loopback mode, false to disable it.
 * @return 0 on success, or a non-zero error code from the read/write operations.
 */
int lan8670_loopback(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x4000,
				   setting); // Set/clear bit 14.
}

/**
 * @brief Enables or disables the LAN8670's low power mode.
 *
 *  When enabled, the PMA will be powered down (meaning the device will not be able to 
 *  send or recieve messages). The remainder of the device will remain functional.
 *
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable low power mode, false to disable it.
 * @return 0 on success, or a non-zero error code from the read/write operations.
 */
int lan8670_low_power_mode(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x800,
				   setting); // Set/clear bit 11.
}

/**
 * @brief Electrically isolates the LAN8670 from MII/RMII.
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to isolate the device, false for normal operation.
 * @return 0 on success, or a non-zero error code from the read/write operations.
 */
int lan8670_isolate(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x400,
				   setting); // Set/clear bit 10.
}

/**
 * @brief Enables or disables the LAN8670's collision test mode.
 * 
 *  When enabled, asserting TXEN will cause the COL output to go high
 *  within 512 bit times. Negating TXEN will cause the COL output to go low within 4 bit times. 
 *  This mode should only be enabled when Loopback is enabled.
 *   
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable collision test mode, false to disable it.
 * @return 0 on success, or a non-zero error code from the read/write operations.
 */
int lan8670_collision_test(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x80,
				   setting); // Set/clear bit 7.
}

/**
 * @brief Detects jabber condition on the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param jabber_status Pointer to a boolean variable to store the jabber status.
 * @return 0 on success, or a non-zero error code from the read operation.
 */
int lan8670_detect_jabber(lan8670_t *lan, bool *jabber_status)
{
	uint32_t data = 0;
	int status = lan->read(lan->device_address, REG_BASIC_STATUS, &data);
	if (status != 0)
		return status; // ERROR: Read failed.
	*jabber_status = (data & 0x02) !=
			 0; // Check if bit 1 (jabber detection status) is set
	return 0;
}
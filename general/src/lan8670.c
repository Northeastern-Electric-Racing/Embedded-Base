// clang-format off
/*
        LAN8670 Ethernet PHY Driver
        Product Page: https://www.microchip.com/en-us/product/lan8670
        Datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/AIS/ProductDocuments/DataSheets/LAN8670-1-2-Data-Sheet-60001573.pdf

        Author: Blake Jackson
*/
#include "lan8670.h"

#include <stdio.h> // Used for debug()
#include <stdarg.h> // Used for debug()

/* SMI Registers */
#define REG_BASIC_CONTROL 		0x00 // Basic Control Register. (Datasheet pgs. 63-64)
#define REG_BASIC_STATUS  		0x01 // Basic Status Register. (Datasheet pgs. 65-67)
#define REG_PHY_ID1	  	  		0x02 // PHY Identifier 1 Register. (Datasheet pg. 68)
#define REG_PHY_ID2	  	  		0x03 // PHY Identifier 2 Register. (Datasheet pg. 69)
#define REG_MMDCTRL	  	  		0x0D // MMD Control Register. (Datasheet pg. 70)
#define REG_MMDAD	  	  		0x0E // MMD Address Register. (Datasheet pg. 71)
#define REG_STRAP_CTRL0	  		0x12 // Strap Control Register 0. (Datasheet pg. 72)

/* MMD: PMA/PMD Registers */
#define MMD_PMAPMD 0x01 // The PMA/PMD registers are located at MMD address 0x01. (Datasheet pg. 73)
    #define PMAPMD_T1PMAPMDEXTA 0x0012 // BASE-T1 PMA/PMD Extended Ability Register (Datasheet pg. 74)
    #define PMAPMD_T1PMAPMDCTL  0x0834 // BASE-T1 PMA/PMD Control Register (Datasheet pg. 75)
    #define PMAPMD_T1SPMATCL    0x08F9 // 10BASE-T1S PMA Control Register (Datasheet pgs. 76-77)
    #define PMAPMD_T2DPMASTS    0x08FA // 10BASE-T1S PMA Status Register (Datasheet pg. 78)
    #define PMAPMD_T1STSTCTL    0x08FB // 10BASE-T1S Test Mode Control Register (Datasheet pg. 79)

/* MMD: PCS Registers */
#define MMD_PCS 0x03  // The PCS registers are located at MMD address 0x03. (Datasheet pg. 80)
    #define PCS_T1SPCSCTL   	0x08F3 // 10BASE-T1S PCS Control Register (Datasheet pg. 81)
    #define PCS_T1SPCSSTS   	0x08F4 // 10BASE-T1S PCS Status Register (Datasheet pg. 82)
    #define PCS_T1SPCSDIAG1 	0x08F5 // 10BASE-T1S PCS Diagnostic Register 1 (Datasheet pg. 83)
    #define PCS_T1SPCSDIAG2 	0x08F6 // 10BASE-T1S PCS Diagnostic Register 2 (Datasheet pg. 84)

/* MMD: Miscellaneous Registers */
#define MMD_MISC 0x1F // The miscellaneous registers are located at MMD address 0x1F. (Datasheet pg. 85)
    #define MISC_CFGPRTCTL   	0x0F // Configuration Port Control Register (Datasheet pg. 88)
    #define MISC_CTRL1       	0x10 // Control 1 Register (Datasheet pg. 89)
    #define MISC_PINCTRL     	0x11 // Pin Control Register (Datasheet pgs. 90-91)
    #define MISC_STS1        	0x18 // Status 1 Register (Datasheet pgs. 92-94)
    #define MISC_STS2        	0x19 // Status 2 Register (Datasheet pg. 95)
    #define MISC_STS3        	0x1A // Status 3 Register (Datasheet pg. 96)
    #define MISC_IMSK1       	0x1C // Interrupt Mask 1 Register (Datasheet pgs. 97-99)
    #define MISC_IMSK2       	0x1D // Interrupt Mask 2 Register (Datasheet pgs. 100-101)
    #define MISC_CTRCTRL     	0x20 // Counter Control Register (Datasheet pg. 102)
    #define MISC_TOCNTH      	0x24 // Transmit Opportunity Count (High) Register (Datasheet pg. 103)
    #define MISC_TOCNTL      	0x25 // Transmit Opportunity Count (Low) Register (Datasheet pg. 104)
    #define MISC_BCNCNTH		0x26 // BEACON Count (High) Register (Datasheet pg. 105)
	#define MISC_BCNCNTL     	0x27 // BEACON Count (Low) Register (Datasheet pg. 106)
    #define MISC_MULTID0     	0x30 // PLCA Multiple ID 0 Register (Datasheet pg. 107)
    #define MISC_MULTID1     	0x31 // PLCA Multiple ID 1 Register (Datasheet pg. 108)
    #define MISC_MULTID2     	0x32 // PLCA Multiple ID 2 Register (Datasheet pg. 109)
    #define MISC_MULTID3     	0x33 // PLCA Multiple ID 3 Register (Datasheet pg. 110)
    #define MISC_PRTMGMT2    	0x3D // Port Management 2 Register (Datasheet pg. 111)
    #define MISC_IWDTOH      	0x3E // Inactivity Watchdog Timeout (High) Register (Datasheet pg. 112)
    #define MISC_IWDTOL      	0x3F // Inactivity Watchdog Timeout (Low) Register (Datasheet pg. 113)
    #define MISC_TXMCTL      	0x40 // Transmit Match Control Register (Datasheet pg. 114)
    #define MISC_TXMPATH     	0x41 // Transmit Mtach Pattern (High) Register (Datasheet pg. 115)
    #define MISC_TXMPATL     	0x42 // Transmit Match Pattern (Low) Register (Datasheet pg. 116)
    #define MISC_TXMDLY      	0x49 // Transmit Mtached Packet Delay Register (Datasheet pg. 117)
    #define MISC_RXMCTL      	0x50 // Receive Match Control Register (Datasheet pg. 118)
    #define MISC_RXMPATH     	0x51 // Recieve Match Pattern (High) Register (Datasheet pg. 119)
    #define MISC_RXMPATL     	0x52 // Recieve Match Pattern (Low) Register (Datasheet pg. 120)
    #define MISC_RXMDLY      	0x59 // Recieve Matched Packet Delay Register (Datasheet pg. 121)
    #define MISC_CBSSPTHH    	0x60 // Credit Based Shaper Stop Threshold (High) Register (Datasheet pg. 122)
    #define MISC_CBSSPTHL    	0x61 // Credit Based Shaper Stop Threshold (Low) Register (Datasheet pg. 123)
    #define MISC_CBSSTTHH    	0x62 // Credit Based Shaper Start Threshold (High) Register (Datasheet pg. 124)
    #define MISC_CBSSTTHL    	0x63 // Credit Based Shaper Start Threshold (Low) Register (Datasheet pg. 125)
    #define MISC_CBSSLPCTL   	0x64 // Credit Based Shaper Slope Control Register (Datasheet pg. 126)
    #define MISC_CBSTPLMTH   	0x65 // Credit Based Shaper Top Limit (High) Register (Datasheet pg. 127)
    #define MISC_CBSTPLMTL   	0x66 // Credit Based Shaper Top Limit (Low) Register (Datasheet pg. 128)
    #define MISC_CBSBTLMTH   	0x67 // Credit Based Shaper Bottom Limit (High) Register (Datasheet pg. 129)
    #define MISC_CBSBTLMTL   	0x68 // Credit Based Shaper Bottom Limit (Low) Register (Datasheet pg. 130)
    #define MISC_CBSCRCTRH   	0x69 // Credit Based Shaper Credit Coutner (High) Register (Datasheet pg. 131)
    #define MISC_CBSCRCTRL   	0x6A // Credit Based Shaper Credit Counter (Low) Register (Datasheet pg. 132)
    #define MISC_CBSCTRL     	0x6B // Credit Based Shaper Control Register (Datasheet pg. 133)
    #define MISC_PLCASKPTCL  	0x70 // PLCA Skip Control Register (Datasheet pg. 134)
    #define MISC_PLCATOSKP   	0x71 // PLCA Transmit Opportunity Skip Register (Datasheet pg. 135)
    #define MISC_ACMACTL     	0x74 // Application Controlled Media Access Control Register (Datasheet pg. 136)
    #define MISC_SLPCTL0     	0x80 // Sleep Control 0 Register (Datasheet pgs. 137-138)
    #define MISC_SLPCTL1     	0x81 // Sleep Control 1 Register (Datasheet pgs. 139-140)
    #define MISC_CDCTL0      	0x87 // Collision Detector Control 0 Register (Datasheet pg. 141)
    #define MISC_SQICTL0     	0xA0 // SQI Control Register (Datasheet pg. 142)
    #define MISC_SQISTS0     	0xA1 // SQI Status 0 Register (Datasheet pgs. 143-144)
    #define MISC_SQICFG0     	0xAA // SQI Configuration 0 Register (Datasheet pg. 145)
    #define MISC_SQICFG2     	0xAC // SQI Configuration 2 Register (Datasheet pg. 146)
    #define MISC_PADCTRL3    	0xCB // Pad Control 3 Register (Datasheet pg. 147)
    #define MISC_ANALOG5     	0xD5 // Analog Control 5 Register (Datasheet pg. 148)
    #define MISC_MIDVER      	0xCA00 // OPEN Alliance Map ID and Version Register (Datasheet pg. 149)
    #define MISC_PLCA_CTRL0  	0xCA01 // PLCA Control 0 Register (Datasheet pg. 150)
    #define MISC_PLCA_CTRL1  	0xCA02 // PLCA Control 1 Register (Datasheet pg. 151)
    #define MISC_PLCA_STS    	0xCA03 // PLCA Status Register (Datasheet pg. 152)
    #define MISC_PLCA_TOTMR  	0xCA04 // PLCA Transmit Opportunity Timer Register (Datasheet pg. 153)
    #define MISC_PLCA_BURST  	0xCA05 // PLCA Burst Mode Register (Datasheet pg. 154)


/**
 * @brief Prints a LAN8670 debug message. Only works if debugging is enabled.
 * @param lan Pointer to the lan8670_t instance.
 * @param format Printf-style format string.
 * @param ... Variable arguments for the format string.
 */
static void debug(lan8670_t *lan, const char *format, ...) {
    if (!lan->debug) return; // Return if debugging is not enabled
    
    va_list args;
    va_start(args, format);
    printf("[LAN8670] ");
    vprintf(format, args);
    va_end(args);
}

/**
 * @brief Helper function. Modifies a specific bit in a register of the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param reg The register address to modify.
 * @param bit_mask The bit mask to modify.
 * @param setting true to set the bit, false to clear it.
 * @return 0 on success, or a non-zero error code.
 */
static int modify_register_bit(lan8670_t *lan, int reg, uint32_t bit_mask, bool setting)
{
	uint32_t data = 0;
	int status = lan->read(lan->device_address, reg, &data);
	if (status != 0) {
		debug("ERROR D1: modify_register_bit() failed when trying to read register %X with lan->read(). (Status: %d)\n",reg,status);
		return status; // ERROR: Read failed.
	}

	if (setting) {
		data |= bit_mask; // Set the bit specified by bit_mask
	} else {
		data &= ~bit_mask; // Clear the bit specified by bit_mask
	}

	status = lan->write(lan->device_address, reg, &data);
	if (status != 0) {
		debug("ERROR D2: modify_register_bit() failed when trying to write register %X with lan->write(). (Status: %d)\n",reg,status);
		return status;
	}
}

/**
 * @brief Helper function. Reads a register from the LAN8670's MMD interface.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param data Buffer to store the read data.
 * @return 0 on success, or a non-zero error code.
 */
static int read_mmd_register(lan8670_t *lan, uint16_t mmd_addr, uint16_t register_offset, uint16_t *data)
{
	/* Tell the MMDCTRL register what MMD device you intend to access (either PMA/PMD, PCS, or MISC). */
	uint16_t mmd_ctrl = mmd_addr & 0x1F;
	int status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0) {
		debug("ERROR D3: read_mmd_register() failed when trying to write REG_MMDCTRL with lan->write(). This was done while trying to read register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}

	/* Tell the MMDAD register what specific register you want to access (by writing the register offset) */
	status = lan->write(lan->device_address, REG_MMDAD, register_offset);
	if (status != 0) {
		debug("ERROR D4: read_mmd_register() failed when trying to write REG_MMDAD with lan->write(). This was done while trying to read register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}

	/* Set the MMD function to 'Data - No post increment' */
	mmd_ctrl = (mmd_addr & 0x1F) | (1 << 14); // Set FNCTN[1:0] to 01 (see page 70 of datasheet).
	status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0) {
		debug("ERROR D5: read_mmd_register() failed when trying to write REG_MMDCTRL with lan->write(). This was done while trying to read register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}

	/* Read data from MMDAD */
	status = lan->read(lan->device_address, REG_MMDAD, data);
	if (status != 0) {
		debug("ERROR D6: read_mmd_register() failed when trying to read REG_MMDAD with lan->read(). This was done while trying to read register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}
}
// EXAMPLE USAGE:
// To read the '10BASE-T1S Test Mode Control' register, which is a PMA/PMD register, you would write:
// uint16_t data;
// read_mmd_register(lan, MMD_PMAPMD, MISC_PINCTRL, data);
// (Note: All the register macros are defined at the top of this file.)

/**
 * @brief Helper function. Writes to a register in the LAN8670's MMD interface.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param data The data to be written to the register.
 * @return 0 on success, or a non-zero error code.
 */
static int write_mmd_register(lan8670_t *lan, uint16_t mmd_addr, uint16_t register_offset, uint16_t data)
{
	/* Tell the MMDCTRL register what MMD device you intend to access (either PMA/PMD, PCS, or MISC). */
	uint16_t mmd_ctrl = mmd_addr & 0x1F; /* DEVAD in bits 4:0, FNCTN in bits 15:14 = 00 */
	int status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0) {
		debug("ERROR D7: write_mmd_register() failed when trying to write REG_MMDCTRL with lan->write(). This was done while trying to write register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}

	/* Tell the MMDAD register what specific register you want to access (by writing the register offset) */
	status = lan->write(lan->device_address, REG_MMDAD, register_offset);
	if (status != 0) {
		debug("ERROR D8: write_mmd_register() failed when trying to write REG_MMDAD with lan->write(). This was done while trying to write register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}

	/* Set the MMD function to 'Data - No post increment' */
	mmd_ctrl = (mmd_addr & 0x1F) | (1 << 14); // Set FNCTN[1:0] to 01 (see page 70 of datasheet).
	status = lan->write(lan->device_address, REG_MMDCTRL, mmd_ctrl);
	if (status != 0) {
		debug("ERROR D9: write_mmd_register() failed when trying to write REG_MMDCTRL with lan->write(). This was done while trying to write register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}

	/* Write data to MMDAD */
	status = lan->write(lan->device_address, REG_MMDAD, data);
	if (status != 0) {
		debug("ERROR D10: write_mmd_register() failed when trying to write REG_MMDAD with lan->write(). This was done while trying to write register %X of MMD device %X. (Status: %d).\n",register_offset,mmd_addr,status);
		return status;
	}
}
// EXAMPLE USAGE:
// To write the 'Pin Control Register' register, which is a Miscellaneous register, you would write:
// uint16_t data;
// write_mmd_register(lan, MMD_MISC, MISC_PINCTRL, data);
// (Note: All the register macros are defined at the top of this file.)

/**** API FUNCTIONS ****/

void lan8670_init(lan8670_t *lan, uint32_t device_address, ReadFunction read, WriteFunction write)
{
	lan->write = write;
	lan->read = read;
	lan->device_address = device_address;
}

int lan8670_reset(lan8670_t *lan)
{
	uint32_t data = 0x8000; // Makes bit 15 = 1. This starts a software reset of the PHY.
	int status = lan->write(lan->device_address, REG_BASIC_CONTROL, &data);
	if(status != 0) {
		debug("ERROR D11: lan8670_reset() failed while trying to write REG_BASIC_CONTROL with lan->write() (Status: %d).\n", status);
		return status;
	}
}

int lan8670_loopback(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x4000, setting); // Set/clear bit 14.
}

int lan8670_low_power_mode(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x800, setting); // Set/clear bit 11.
}

int lan8670_isolate(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x400, setting); // Set/clear bit 10.
}

int lan8670_collision_test(lan8670_t *lan, bool setting)
{
	return modify_register_bit(lan, REG_BASIC_CONTROL, 0x80, setting); // Set/clear bit 7.
}

int lan8670_detect_jabber(lan8670_t *lan, bool *jabber_status)
{
	uint32_t data = 0;
	int status = lan->read(lan->device_address, REG_BASIC_STATUS, &data);
	if (status != 0) {
		debug("ERROR D12: lan8670_detect_jabber() failed when trying to read REG_BASIC_STATUS with lan->read() (Status: %d).\n",status);
		return status;
	}
	*jabber_status = (data & 0x02) != 0; // Check if bit 1 (jabber detection status) is set
	return 0;
}

int lan8670_plca_reset(lan8670_t *lan)
{
	uint32_t data = 0x4000; // Makes bit 14 = 1. This starts a software reset of the PLCA reconciliation sublayer.
	int status = write_mmd_register(lan, MMD_MISC, MISC_PLCA_CTRL0, data);
	if(status != 0) {
		debug("ERROR D13: lan8670_plca_reset() failed while calling write_mmd_register() (Status: %d).\n", status);
		return status;
	}
}

// clang-format on
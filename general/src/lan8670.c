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
#include "u_tx_debug.h"

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
 * @brief Prints a LAN8670 debug message. Only works if debugging is enabled (this can be toggled in the lan8670_t struct).
 * @param lan Pointer to the lan8670_t instance.
 * @param format Printf-style format string.
 * @param ... Variable arguments for the format string.
 */
static void debug(lan8670_t *lan, const char *format, ...) {
    if (!lan->debug) return; // Return if debugging is not enabled
    
    va_list args;
    va_start(args, format);
    printf("[LAN8670] "); // Every debug message starts with this tag
    vprintf(format, args);
    va_end(args);
}

/**
 * @brief Reads a field of bits from a register.
 * @param lan Pointer to the lan8670_t instance.
 * @param reg The register address to read from.
 * @param start The starting bit position (must be between 0 and 31, since the SMI registers are 32 bits).
 * @param end The ending bit position (must be between 0 and 31, since the SMI registers are 32 bits).
 * @param value Pointer to store the read value.
 * @return Status.
 */
static int read_register_field(lan8670_t *lan, int reg, int start, int end, uint32_t *value)
{
    /* Validate bit range (If the first bit is greater than the last bit, the field is invalid.) */
    if (start > end) {
        debug(lan, "ERROR 1000: read_register_field() invalid bit range: start (%d) > end (%d)\n", start, end);
        return LAN8670_STATUS_ERROR;
    }
    if (start < 0 || end > 31) {
        debug(lan, "ERROR 1001: read_register_field() bit range out of bounds: start=%d, end=%d\n", start, end);
        return LAN8670_STATUS_ERROR;
    }

    /* Read the register */
    uint32_t data = 0;
    int status = lan->IO.ReadReg(lan->DevAddr, reg, &data);
    if (status != 0) {
        debug(lan, "ERROR 1002: read_register_field() failed to read register 0x%X (Status: %d)\n", reg, status);
        return LAN8670_STATUS_READ_ERROR;
    }

    /* Calculate field width and mask */
    int field_width = end - start + 1;
    uint32_t mask = ((1U << field_width) - 1) << start;

    /* Extract and shift the field */
    *value = (data & mask) >> start;
    return LAN8670_STATUS_OK;
}
// EXAMPLE USAGE:
// To read bits 8 through 15 of the Basic Status Register:
// uint32_t value;
// read_register_field(lan, REG_BASIC_STATUS, 8, 15, &value);

/**
 * @brief Writes a field of bits to a register.
 * @param lan Pointer to the lan8670_t instance.
 * @param reg The register address to write to.
 * @param start The starting bit position (must be between 0 and 31, since the SMI registers are 32 bits).
 * @param end The ending bit position (must be between 0 and 31, since the SMI registers are 32 bits).
 * @param value The value to write to the field.
 * @return Status.
 */
static int write_register_field(lan8670_t *lan, int reg, int start, int end, uint32_t value)
{
    /* Validate bit range (If the first bit is greater than the last bit, the field is invalid.) */
    if (start > end) {
        debug(lan, "ERROR 2000: write_register_field() invalid bit range: start (%d) > end (%d)\n", start, end);
        return LAN8670_STATUS_ERROR;
    }
    if (start < 0 || end > 31) {
        debug(lan, "ERROR 2001: write_register_field() bit range out of bounds: start=%d, end=%d\n", start, end);
        return LAN8670_STATUS_ERROR;
    }

    /* Calculate field width and validate value */
    int field_width = end - start + 1;
    uint32_t max_value = (1U << field_width) - 1;
    if (value > max_value) {
        debug(lan, "ERROR 2002: write_register_field() value 0x%X exceeds maximum 0x%X for %d-bit field\n", 
              value, max_value, field_width);
        return LAN8670_STATUS_ERROR;
    }

    /* Read the current register value */
    uint32_t data = 0;
    int status = lan->IO.ReadReg(lan->DevAddr, reg, &data);
    if (status != 0) {
        debug(lan, "ERROR 3000: write_register_field() failed to read register 0x%X (Status: %d)\n", reg, status);
        return LAN8670_STATUS_READ_ERROR;
    }

    /* Calculate mask and update the field */
    uint32_t mask = ((1U << field_width) - 1) << start;
    data = (data & ~mask) | ((value << start) & mask);

    /* Write back the modified value */
    status = lan->IO.WriteReg(lan->DevAddr, reg, data);
    if (status != 0) {
        debug(lan, "ERROR 3001: write_register_field() failed to write register 0x%X (Status: %d)\n", reg, status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    return LAN8670_STATUS_OK;
}
// EXAMPLE USAGE:
// To write bits 0 through 7 of the Basic Control Register:
// uint32_t value = 0b01101001;
// write_register_field(lan, REG_BASIC_CONTROL, 0, 7, value);

/**
 * @brief Reads an MMD register.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param value Pointer to store the read value.
 * @return Status.
 */
static int mmd_read_register(lan8670_t *lan, uint16_t mmd_addr, uint16_t register_offset, uint16_t *value)
{
    /* Tell the MMDCTRL register what MMD device you intend to access */
    uint16_t mmd_ctrl = mmd_addr & 0x1F;
    int status = lan->IO.WriteReg(lan->DevAddr, REG_MMDCTRL, mmd_ctrl);
    if (status != 0) {
        debug(lan, "ERROR 4000: mmd_read_register() failed when writing REG_MMDCTRL (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    /* Tell the MMDAD register what specific register you want to access */
    status = lan->IO.WriteReg(lan->DevAddr, REG_MMDAD, register_offset);
    if (status != 0) {
        debug(lan, "ERROR 4001: mmd_read_register() failed when writing REG_MMDAD (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    /* Set the MMD function to 'Data - No post increment' */
    mmd_ctrl = (mmd_addr & 0x1F) | (1 << 14);
    status = lan->IO.WriteReg(lan->DevAddr, REG_MMDCTRL, mmd_ctrl);
    if (status != 0) {
        debug(lan, "ERROR 4002: mmd_read_register() failed when writing REG_MMDCTRL (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    /* Read data from MMDAD */
    uint32_t read = 0;
    status = lan->IO.ReadReg(lan->DevAddr, REG_MMDAD, &read);
    if (status != 0) {
        debug(lan, "ERROR 4003: mmd_read_register() failed when reading REG_MMDAD (Status: %d)\n", status);
        return LAN8670_STATUS_READ_ERROR;
    }

    *value = (uint16_t)(read & 0xFFFF); // Truncate to 16 bits
    return LAN8670_STATUS_OK;
}
// EXAMPLE USAGE:
// To read the Pin Control Register (which is a Miscellaneous register):
// uint16_t value;
// mmd_read_register(lan, MMD_MISC, MISC_PINCTRL, &value);

/**
 * @brief Writes an MMD register.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param value The value to write to the register.
 * @return Status.
 */
static int mmd_write_register(lan8670_t *lan, uint16_t mmd_addr, uint16_t register_offset, uint16_t value)
{
    /* Tell the MMDCTRL register what MMD device you intend to access */
    uint16_t mmd_ctrl = mmd_addr & 0x1F;
    int status = lan->IO.WriteReg(lan->DevAddr, REG_MMDCTRL, mmd_ctrl);
    if (status != 0) {
        debug(lan, "ERROR 5000: mmd_write_register() failed when writing REG_MMDCTRL (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    /* Tell the MMDAD register what specific register you want to access */
    status = lan->IO.WriteReg(lan->DevAddr, REG_MMDAD, register_offset);
    if (status != 0) {
        debug(lan, "ERROR 5001: mmd_write_register() failed when writing REG_MMDAD (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    /* Set the MMD function to 'Data - No post increment' */
    mmd_ctrl = (mmd_addr & 0x1F) | (1 << 14);
    status = lan->IO.WriteReg(lan->DevAddr, REG_MMDCTRL, mmd_ctrl);
    if (status != 0) {
        debug(lan, "ERROR 5002: mmd_write_register() failed when writing REG_MMDCTRL (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    /* Write data to MMDAD */
    status = lan->IO.WriteReg(lan->DevAddr, REG_MMDAD, value);
    if (status != 0) {
        debug(lan, "ERROR 5003: mmd_write_register() failed when writing REG_MMDAD (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    return LAN8670_STATUS_OK;
}
// EXAMPLE USAGE:
// To write to the PLCA Control 1 Register (which is a Miscellaneous register):
// uint16_t value = 0x01;
// mmd_write_register(lan, MMD_MISC, MISC_PLCA_CTRL1, value);

/**
 * @brief Reads a field of bits from an MMD register.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param start The starting bit position (must be between 0 and 15, since the MMD registers are 16 bits).
 * @param end The ending bit position (must be between 0 and 15, since the MMD registers are 16 bits).
 * @param value Pointer to store the read value.
 * @return Status.
 */
static int mmd_read_register_field(lan8670_t *lan, uint16_t mmd_addr, uint16_t register_offset, int start, int end, uint16_t *value)
{

    /* Validate bit range */
    if (start > end) {
        debug(lan, "ERROR 6000: mmd_read_register_field() invalid bit range: start (%d) > end (%d)\n", start, end);
        return LAN8670_STATUS_ERROR;
    }
    if (start < 0 || end > 15) {
        debug(lan, "ERROR 6001: mmd_read_register_field() bit range out of bounds: start=%d, end=%d\n", start, end);
        return LAN8670_STATUS_ERROR;
    }

    /* Read the full register */
    uint16_t data = 0;
    int status = mmd_read_register(lan, mmd_addr, register_offset, &data);
    if (status != 0) {
        debug(lan, "ERROR 6002: mmd_read_register_field() failed to read register (Status: %d)\n", status);
        return LAN8670_STATUS_READ_ERROR;
    }

    /* Calculate field width and mask */
    int field_width = end - start + 1;
    uint16_t mask = ((1U << field_width) - 1) << start;

    /* Extract and shift the field */
    *value = (data & mask) >> start;
    return LAN8670_STATUS_OK;
}
// EXAMPLE USAGE:
// To read bits 0 through 7 of the PLCA Control 1 Register (which is a Miscellaneous register):
// uint16_t value;
// mmd_read_register_field(lan, MMD_MISC, MISC_PLCA_CTRL1, 0, 7, &value);

/**
 * @brief Writes a field of bits to an MMD register.
 * @param lan Pointer to the lan8670_t instance.
 * @param mmd_addr The MMD device address (PMA/PMD, PCS, or MISC).
 * @param register_offset The offset of the register within the MMD group.
 * @param start The starting bit position (must be between 0 and 15, since the MMD registers are 16 bits).
 * @param end The ending bit position (must be between 0 and 15, since the MMD registers are 16 bits).
 * @param value The value to write to the field.
 * @return Status.
 */
static int mmd_write_register_field(lan8670_t *lan, uint16_t mmd_addr, uint16_t register_offset, int start, int end, uint16_t value)
{
    /* Validate bit range */
    if (start > end) {
        debug(lan, "ERROR 7000: mmd_write_register_field() invalid bit range: start (%d) > end (%d)\n", start, end);
        return LAN8670_STATUS_ERROR;
    }
    if (start < 0 || end > 15) {
        debug(lan, "ERROR 7001: mmd_write_register_field() bit range out of bounds: start=%d, end=%d\n", start, end);
        return LAN8670_STATUS_ERROR;
    }

    /* Calculate field width and validate value */
    int field_width = end - start + 1;
    uint16_t max_value = (1U << field_width) - 1;
    if (value > max_value) {
        debug(lan, "ERROR 7002: mmd_write_register_field() value 0x%X exceeds maximum 0x%X for %d-bit field\n", 
              value, max_value, field_width);
        return LAN8670_STATUS_ERROR;
    }

    /* Read current register value */
    uint16_t data = 0;
    int status = mmd_read_register(lan, mmd_addr, register_offset, &data);
    if (status != 0) {
        debug(lan, "ERROR 7003: mmd_write_register_field() failed to read register (Status: %d)\n", status);
        return LAN8670_STATUS_READ_ERROR;
    }

    /* Calculate mask and update the field */
    uint16_t mask = ((1U << field_width) - 1) << start;
    data = (data & ~mask) | ((value << start) & mask);

    /* Write back the modified value */
    status = mmd_write_register(lan, mmd_addr, register_offset, data);
    if (status != 0) {
        debug(lan, "ERROR 7004: mmd_write_register_field() failed to write register (Status: %d)\n", status);
        return LAN8670_STATUS_WRITE_ERROR;
    }

    return LAN8670_STATUS_OK;
}
// EXAMPLE USAGE:
// To write bits 0 through 7 of the PLCA Control 1 Register (which is a Miscellaneous register):
// uint16_t value = 0b01101001;
// mmd_write_register_field(lan, MMD_MISC, MISC_PLCA_CTRL1, 0, 7, value);

/**** API FUNCTIONS ****/

int32_t LAN8670_Init(lan8670_t *lan)
{
    // Set the device address to the SMIADR[4:0] field of the Strap Control 0 Register.
    uint32_t buffer = 0;
	int32_t status = read_register_field(lan, REG_STRAP_CTRL0, 0, 4, &buffer);
    if(status != 0) {
        debug(lan, "ERROR 0000: LAN8670_Init() failed to read Strap Control Register 0 (Status: %d)\n", status);
        return LAN8670_STATUS_READ_ERROR;
    }
    lan->DevAddr = buffer;

	lan->debug = false; // Default to no debugging.

    return LAN8670_STATUS_OK;
}

int32_t LAN8670_RegisterBusIO(lan8670_t *lan, lan8670_IOCtx_t *ioctx)
{
    if(!lan || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick) {
        return LAN8670_STATUS_ERROR; // Invalid parameters
    }

    lan->IO.Init = ioctx->Init;
    lan->IO.DeInit = ioctx->DeInit;
    lan->IO.WriteReg = ioctx->WriteReg;
    lan->IO.ReadReg = ioctx->ReadReg;
    lan->IO.GetTick = ioctx->GetTick;

    return LAN8670_STATUS_OK;
}

int32_t LAN8670_Reset(lan8670_t *lan)
{
    // Set bit 15 in the Basic Control Register, and clear all other bits.
    // This starts a software reset of the PHY.
	return lan->IO.WriteReg(lan->DevAddr, REG_BASIC_CONTROL, 0x8000);
}

int32_t LAN8670_Loopback(lan8670_t *lan, bool setting)
{
    // Modify bit 14 of the Basic Control Register to whatever 'setting' is.
	return write_register_field(lan, REG_BASIC_CONTROL, 14, 14, setting);
}

int32_t LAN8670_Low_Power_Mode(lan8670_t *lan, bool setting)
{
    // Modify bit 11 of the Basic Control Register to whatever 'setting' is.
	return write_register_field(lan, REG_BASIC_CONTROL, 11, 11, setting);
}

int32_t LAN8670_Isolate(lan8670_t *lan, bool setting)
{
    // Modify bit 10 of the Basic Control Register to whatever 'setting' is.
    return write_register_field(lan, REG_BASIC_CONTROL, 10, 10, setting);
}

int32_t LAN8670_Collision_Test(lan8670_t *lan, bool setting)
{
    // Modify bit 7 of the Basic Control Register to whatever 'setting' is.
    return write_register_field(lan, REG_BASIC_CONTROL, 7, 7, setting);
}

int32_t LAN8670_Collision_Detection(lan8670_t *lan, bool setting) {
    // Modify bit 15 of the Collision Detector Control 0 Register to whatever 'setting' is.
    return mmd_write_register_field(lan, MMD_MISC, MISC_CDCTL0, 15, 15, setting);
}

int32_t LAN8670_PLCA_On(lan8670_t *lan, bool setting)
{
    // Set/clear bit 15 of the PLCA Control 0 Register to whatever 'setting' is.
    return mmd_write_register_field(lan, MMD_MISC, MISC_PLCA_CTRL0, 15, 15, setting);
}

int32_t LAN8670_PLCA_Reset(lan8670_t *lan)
{
    // Set bit 14 in the PLCA Control 0 Register, and clear all other bits.
    // This starts a software reset of the PLCA reconciliation sublayer. 
	return mmd_write_register(lan, MMD_MISC, MISC_PLCA_CTRL0, 0x4000);
}

int32_t LAN8670_PLCA_Set_Node_Count(lan8670_t *lan, uint8_t node_count) 
{
    // Modify bits 8-15 of the PLCA Control 1 Register to whatever 'node_count' is.
	return mmd_write_register_field(lan, MMD_MISC, MISC_PLCA_CTRL1, 8, 15, node_count);
}

int32_t LAN8670_PLCA_Set_Node_Id(lan8670_t *lan, uint8_t id) 
{
    // Modify bits 0-7 of the PLCA Control 1 Register to whatever 'id' is.
    return mmd_write_register_field(lan, MMD_MISC, MISC_PLCA_CTRL1, 0, 7, id);
}

/* false=The PLCA reconciliation sublayer is not regularly receiving or transmitting the BEACON, true=The PLCA reconciliation sublayer is regularly receiving or transmitting the BEACON. */
int32_t LAN8670_PLCA_Get_Status(lan8670_t *lan, bool *status) {
    uint16_t reading = 0;
    int error = mmd_read_register_field(lan, MMD_MISC, MISC_PLCA_STS, 15, 15, &reading);
    if(error != LAN8670_STATUS_OK) {
        PRINTLN_ERROR("Failed to call mmd_read_register_field() (Status: %d).", error);
        return error;
    }

    if(reading == 1) {
        *status = true;
    } else {
        *status = false;
    }

    return LAN8670_STATUS_OK;
}

/* Reads the value of the TOMTR register. Should be 32 bit-times by default. */
int32_t LAN8670_PLCA_ReadTOTMR(lan8670_t *lan, uint8_t *buffer) {
    uint16_t reading = 0;
    int status = mmd_read_register_field(lan, MMD_MISC, MISC_PLCA_TOTMR, 0, 7, &reading);
    if(status != LAN8670_STATUS_OK) {
        PRINTLN_ERROR("Failed to call mmd_read_register_field() (Status: %d).", status);
        return status;
    }

    *buffer = (uint8_t)reading;
    return LAN8670_STATUS_OK;
}

/* Writes the value of the TOMTOR register. */
int32_t LAN8670_PLCA_WriteTOTMR(lan8670_t *lan, uint8_t data) {
    int status = mmd_write_register_field(lan, MMD_MISC, MISC_PLCA_TOTMR, 0, 7, (uint16_t)data);
    if(status != LAN8670_STATUS_OK) {
        PRINTLN_ERROR("Failed to call mmd_write_register_field() (Status: %d).", status);
        return status;
    }

    return LAN8670_STATUS_OK;
}

int32_t LAN8670_Get_Link_State(lan8670_t *lan, uint8_t *state)
{   
    // Read bit 2 of the Basic Status register. (Note: For the LAN8670, this will always be 1.)
    uint32_t value = 0;
    int status = read_register_field(lan, REG_BASIC_STATUS, 2, 2, &value);
    if (status != LAN8670_STATUS_OK) {
        debug(lan, "ERROR 8000: LAN8670_Get_Link_State() failed to read Basic Status Register (Status: %d)\n", status);
        return status;
    }

    // Store the link state in the provided pointer.
    *state = (uint8_t)value;
    return LAN8670_STATUS_OK;
}

// clang-format on
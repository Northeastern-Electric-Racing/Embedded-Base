// clang-format off
/*
        LAN8670 Ethernet PHY Driver
        Product Page (with datasheet):
        https://www.microchip.com/en-us/product/lan8670
*/

#include <stdint.h>
#include <stdbool.h>

/* REGISTER ADDRESSES */
#define REG_BASIC_CONTROL 0x00 // Basic Control Register.
#define REG_BASIC_STATUS  0x01
#define REG_PHY_ID1	  0x02
#define REG_PHY_ID2	  0x03
#define REG_MMDCTRL	  0x0D
#define REG_MMDAD	  0x0E
#define REG_STRAP_CTRL0	  0x12

/* MMD REGISTER ADDRESSES */
#define REGMMD_PMAPMD 0x01 // The PMA/PMD registers are located at MMD address 0x01.
        #define PMAPMD_T1PMAPMDEXTA 0x0012 // BASE-T1 PMA/PMD Extended Ability Register
        #define PMAPMD_T1PMAPMDCTL  0x0834 // BASE-T1 PMA/PMD Control Register
        #define PMAPMD_T1SPMATCL    0x08F9 // 10BASE-T1S PMA Control Register
        #define PMAPMD_T2DPMASTS    0x08FA // 10BASE-T1S PMA Status Register
        #define PMAPMD_T1STSTCTL    0x08FB // 10BASE-T1S Test Mode Control Register
#define REGMMD_PCS 0x03  // The PCS registers are located at MMD address 0x03.
        #define PCS_T1SPCSCTL   0x08F3 // 10BASE-T1S PCS Control Register
        #define PCS_T1SPCSSTS   0x08F4 // 10BASE-T1S PCS Status Register
        #define PCS_T1SPCSDIAG1 0x08F5 // 10BASE-T1S PCS Diagnostic Register 1
        #define PCS_T1SPCSDIAG2 0x08F6 // 10BASE-T1S PCS Diagnostic Register 2
#define REGMMD_MISC 0x1F // The miscellaneous registers are located at MMD address 0x1F.
        #define MISC_CFGPRTCTL  0x0F
        #define MISC_CTRL1      0x10
        #define MISC_PINCTRL    0x11            // add comments to all this stuff later. probably with page numbers in the datasheet
        #define MISC_STS1       0x18
        #define MISC_STS2       0x19
        #define MISC_STS3       0x1A
        #define MISC_IMSK1      0x1C
        #define MISC_IMSK2      0x1D
        #define MISC_CTRCTRL    0x20
        #define MISC_TOCNTH     0x24
        #define MISC_TOCNTL     0x25
        #define MISC_BCNCNTL    0x27
        #define MISC_MULTID0    0x30
        #define MISC_MULTID1    0x31
        #define MISC_MULTID2    0x32
        #define MISC_MULTID3    0x33
        #define MISC_PRTMGMT2   0x3D
        #define MISC_IWDTOH     0x3E
        #define MISC_IWDTOL     0x3F
        #define MISC_TXMCTL     0x40
        #define MISC_TXMPATH    0x41
        #define MISC_TXMPATL    0x42
        #define MISC_TXMDLY     0x49
        #define MISC_RXMCTL     0x50
        #define MISC_RXMPATH    0x51
        #define MISC_RXMPATL    0x52
        #define MISC_RXMDLY     0x59
        #define MISC_CBSSPTHH   0x60
        #define MISC_CBSSPTHL   0x61
        #define MISC_CBSSTTHH   0x62
        #define MISC_CBSSTTHL   0x63
        #define MISC_CBSSLPCTL  0x64
        #define MISC_CBSTPLMTH  0x65
        #define MISC_CBSTPLMTL  0x66
        #define MISC_CBSBTLMTH  0x67
        #define MISC_CBSBTLMTL  0x68
        #define MISC_CBSCRCTRH  0x69
        #define MISC_CBSCRCTRL   0x6A
        #define MISC_CBSCTRL     0x6B
        #define MISC_PLCASKPTCL  0x70
        #define MISC_PLCATOSKP   0x71
        #define MISC_ACMACTL     0x74
        #define MISC_SLPCTL0     0x80
        #define MISC_SLPCTL1     0x81
        #define MISC_CDCTL0      0x87
        #define MISC_SQICTL0     0xA0
        #define MISC_SQISTS0     0xA1
        #define MISC_SQICFG0     0xAA
        #define MISC_SQICFG2     0xAC
        #define MISC_PADCTRL3    0xCB
        #define MISC_ANALOG5     0xD5
        #define MISC_MIDVER      0xCA00
        #define MISC_PLCA_CTRL0  0xCA01
        #define MISC_PLCA_CTRL1  0xCA02
        #define MISC_PLCA_STS    0xCA03
        #define MISC_PLCA_TOTMR  0xCA04
        #define MISC_PLCA_BURST  0xCA05

/* FUNCTION POINTERS */
typedef int (*ReadFunction)(uint32_t device_address, uint32_t register_address, uint32_t *data); // Function pointer for reading data from the PHY. Analagous to HAL_ETH_ReadPHYRegister() in the STM32 HAL.
typedef int (*WriteFunction)(uint32_t device_address, uint32_t register_address, uint32_t *data); // Function pointer for writing data to the PHY. Analagous to HAL_ETH_WritePHYRegister() in the STM32 HAL.

/* LAN8670 STRUCT */
typedef struct {
	uint32_t device_address; // The address of the LAN8670 device.
	ReadFunction read; // Function to read data from the PHY.
	WriteFunction write; // Function to write data to the PHY.
} lan8670_t;

/**
 * @brief Initializes a LAN8670 instance.
 * @param lan Pointer to the lan8670_t instance.
 * @param device_address The address of the LAN8670.
 * @param read Function pointer for reading data from the LAN8670.
 * @param write Function pointer for writing data to the LAN8670.
 */
void lan8670_init(lan8670_t *lan, uint32_t device_address, ReadFunction read, WriteFunction write); // Initializes a LAN8670 instance.

/**
 * @brief Performs a software reset of the LAN8670 Ethernet PHY.
 * @param lan Pointer to the lan8670_t instance.
 * @return 0 on success, or a non-zero error code.
 */
int lan8670_reset(lan8670_t *lan); // Performs a software reset of the LAN8670 Ethernet PHY.

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
int lan8670_loopback(lan8670_t *lan, bool setting); // Enables or disables loopback mode on the LAN8670.

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
int lan8670_low_power_mode(lan8670_t *lan, bool setting); // Enables or disables the LAN8670's low power mode.

/**
 * @brief Electrically isolates the LAN8670 from MII/RMII.
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to isolate the device, false for normal operation.
 * @return 0 on success, or a non-zero error code from the read/write operations.
 */
int lan8670_isolate(lan8670_t *lan, bool setting); // Electrically isolates the LAN8670 from MII/RMII.

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
int lan8670_collision_test(lan8670_t *lan, bool setting); // Enables or disables the LAN8670's collision test mode.

/**
 * @brief Detects jabber condition on the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param jabber_status Pointer to a boolean variable to store the jabber status.
 * @return 0 on success, or a non-zero error code from the read operation.
 */
int lan8670_detect_jabber(lan8670_t *lan, bool *jabber_status); // Detects jabber condition on the LAN8670.
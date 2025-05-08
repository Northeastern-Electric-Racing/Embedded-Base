// clang-format off
/*
        LAN8670 Ethernet PHY Driver
        Product Page: https://www.microchip.com/en-us/product/lan8670
        Datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/AIS/ProductDocuments/DataSheets/LAN8670-1-2-Data-Sheet-60001573.pdf

        Author: Blake Jackson
*/

#include <stdint.h>
#include <stdbool.h>

/* FUNCTION POINTERS */
typedef int (*ReadFunction)(uint32_t device_address, uint32_t register_address, uint32_t *data); // Function pointer for reading data from the PHY. Analagous to HAL_ETH_ReadPHYRegister() in the STM32 HAL.
typedef int (*WriteFunction)(uint32_t device_address, uint32_t register_address, uint32_t data); // Function pointer for writing data to the PHY. Analagous to HAL_ETH_WritePHYRegister() in the STM32 HAL.

/* LAN8670 STRUCT */
typedef struct {
	uint32_t device_address; // The address of the LAN8670 device.
	ReadFunction read; // Function to read data from the PHY.
	WriteFunction write; // Function to write data to the PHY.
        bool debug; // Enables printfs for debugging. Defaults to false.
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

/**
 * @brief Enables or disables Physical Layer Collision Avoidence (PLCA).
 * 
 *  Note: When PLCA is enabled on a properly configured mixing segment, no collisions should occur
 *  on the physical layer. It is therefore recommended to disable physical layer collision detection to
 *  achieve a higher level of noise tolerance.
 *   
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable PLCA, false to disable it.
 * @return 0 on success, or a non-zero error code from the read/write operations.
 */
int lan8670_plca(lan8670_t *lan, bool setting);
// clang-format on
// clang-format off
/*
        LAN8670 Ethernet PHY Driver
        Product Page (with datasheet):
        https://www.microchip.com/en-us/product/lan8670
*/

#include <stdint.h>
#include <stdbool.h>

// note for myself: once i've written all functions for the registers, i gotta write comments to explain what each register does
/* REGISTER ADDRESSES */
#define REG_BASIC_CONTROL 0x00 // Basic Control Register.
#define REG_BASIC_STATUS  0x01
#define REG_PHY_ID1	      0x02
#define REG_PHY_ID2	      0x03
#define REG_MMDCTRL	      0x0D
#define REG_MMDAD	      0x0E
#define REG_STRAP_CTRL0	  0x12

/* FUNCTION POINTERS */
typedef int (*ReadFunction)(uint32_t device_address, uint32_t register_address, uint32_t *data); // Function pointer for reading data from the PHY. Analagous to HAL_ETH_ReadPHYRegister() in the STM32 HAL.
typedef int (*WriteFunction)(uint32_t device_address, uint32_t register_address, uint32_t *data); // Function pointer for writing data to the PHY. Analagous to HAL_ETH_WritePHYRegister() in the STM32 HAL.

/* LAN8670 STRUCT */
typedef struct {
	uint32_t device_address; // The address of the LAN8670 device.
	ReadFunction read; // Function to read data from the PHY.
	WriteFunction write; // Function to write data to the PHY.
} lan8670_t;

/* LAN8670 FUNCTIONS */
void lan8670_init(lan8670_t *lan, uint32_t device_address, ReadFunction read, WriteFunction write); // Initializes a LAN8670 instance.
int lan8670_reset(lan8670_t *lan); // Performs a software reset of the LAN8670 Ethernet PHY.
int lan8670_loopback(lan8670_t *lan, bool setting); // Enables or disables loopback mode on the LAN8670.
int lan8670_low_power_mode(lan8670_t *lan, bool setting); // Enables or disables the LAN8670's low power mode.
int lan8670_isolate(lan8670_t *lan, bool setting); // Electrically isolates the LAN8670 from MII/RMII.
int lan8670_collision_test(lan8670_t *lan, bool setting); // Enables or disables the LAN8670's collision test mode.
int lan8670_detect_jabber(lan8670_t *lan, bool *jabber_status); // Detects jabber condition on the LAN8670.
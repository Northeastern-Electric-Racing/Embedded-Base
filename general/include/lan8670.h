// clang-format off
/*
        LAN8670 Ethernet PHY Driver
        Product Page: https://www.microchip.com/en-us/product/lan8670
        Datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/AIS/ProductDocuments/DataSheets/LAN8670-1-2-Data-Sheet-60001573.pdf

        Author: Blake Jackson
*/

#include <stdint.h>
#include <stdbool.h>

/* STATUS CODES */
#define  LAN8670_STATUS_READ_ERROR            ((int32_t)-5)
#define  LAN8670_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  LAN8670_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  LAN8670_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  LAN8670_STATUS_ERROR                 ((int32_t)-1)
#define  LAN8670_STATUS_OK                    ((int32_t) 0)
#define  LAN8670_STATUS_LINK_DOWN             ((int32_t) 1)

/* FUNCTION POINTERS */
typedef int32_t  (*lan8670_Init_Func) (void); 
typedef int32_t  (*lan8670_DeInit_Func) (void);
typedef int32_t  (*lan8670_ReadReg_Func)   (uint32_t, uint32_t, uint32_t *);
typedef int32_t  (*lan8670_WriteReg_Func)  (uint32_t, uint32_t, uint32_t);
typedef uint32_t  (*lan8670_GetTick_Func)  (void);

/* IO STRUCT */
typedef struct 
{                   
  lan8670_Init_Func      Init; 
  lan8670_DeInit_Func    DeInit;
  lan8670_WriteReg_Func  WriteReg;
  lan8670_ReadReg_Func   ReadReg; 
  lan8670_GetTick_Func   GetTick;   
} lan8670_IOCtx_t;  

/* LAN8670 STRUCT */
typedef struct {
	uint32_t DevAddr; // The address of the LAN8670 device.
        uint32_t Is_Initialized; // Indicates if the device is initialized.
        lan8670_IOCtx_t IO; // I/O context for the device.
        bool debug; // Enables printfs for debugging. Defaults to false.
} lan8670_t;

/**
 * @brief Initializes a LAN8670 instance.
 * @param lan Pointer to the lan8670_t instance.
 * @param DevAddr The device address of the LAN8670. This is a 5-bit value indicated by the PHYAD0-PHYAD4 pins and their pull-up/pull-down configuration.
 * @return Status.
 */
int32_t LAN8670_Init(lan8670_t *lan, uint32_t DevAddr); // Initializes a LAN8670 instance.

/**
 * @brief Performs a software reset of the LAN8670 Ethernet PHY.
 * @param lan Pointer to the lan8670_t instance.
 * @return Status.
 */
int32_t LAN8670_Reset(lan8670_t *lan); // Performs a software reset of the LAN8670 Ethernet PHY.

/**
 * @brief Enables or disables loopback mode on the LAN8670.
 *
 *  When enabled, transmit data (TXD) pins from the
 *  MAC will be looped back onto the receive data (RXD) pins to the MAC. In this mode, no signal is
 *  transmitted onto the network media
 *
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable loopback mode, false to disable it.
 * @return Status.
 */
int32_t LAN8670_Loopback(lan8670_t *lan, bool setting); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Enables or disables the LAN8670's low power mode.
 *
 *  When enabled, the PMA will be powered down (meaning the device will not be able to 
 *  send or recieve messages). The remainder of the device will remain functional.
 *
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable low power mode, false to disable it.
 * @return Status.
 */
int32_t LAN8670_Low_Power_Mode(lan8670_t *lan, bool setting); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Electrically isolates the LAN8670 from MII/RMII.
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to isolate the device, false for normal operation.
 * @return Status.
 */
int32_t LAN8670_Isolate(lan8670_t *lan, bool setting); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Enables or disables the LAN8670's collision test mode.
 * 
 *  When enabled, asserting TXEN will cause the COL output to go high
 *  within 512 bit times. Negating TXEN will cause the COL output to go low within 4 bit times. 
 *  This mode should only be enabled when Loopback is enabled.
 *   
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable collision test mode, false to disable it.
 * @return Status.
 */
int32_t LAN8670_Collision_Test(lan8670_t *lan, bool setting); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Enables or disables collision detection on the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable collision detection, false to disable it.
 * @return Status.
 */
int32_t LAN8670_Collision_Detection(lan8670_t *lan, bool setting);

/**
 * @brief Enables or disables Physical Layer Collision Avoidence (PLCA).
 * 
 *  Note: When PLCA is enabled, collision detection should be disabled.
 *  This can be done using the lan8670_collision_detection() function.
 *  (See page 141 of the datasheet for more information.)
 *   
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable PLCA, false to disable it.
 * @return Status.
 */
int32_t LAN8670_PLCA_On(lan8670_t *lan, bool setting);

/**
 * @brief Resets the PLCA reconciliation sublayer.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @return Status.
 */
int32_t LAN8670_PLCA_Reset(lan8670_t *lan); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Configures the maximum number of nodes supported on the multidrop network.
 *  
 * Proper operation requires that this field be set to at least the 
 * number of nodes that may exist on the network.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param node_count The number of nodes on the network.
 * @return Status.
 */
int32_t LAN8670_PLCA_Set_Node_Count(lan8670_t *lan, uint8_t node_count);

/**
 * @brief Sets the ID of the PLCA node.
 * 
 * This ID must be unique for each node on the network.
 * The ID for the controller node must be 0.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param id The ID of the PLCA node (0-31).
 * @return Status.
 */
int32_t LAN8670_PLCA_Set_Node_Id(lan8670_t *lan, uint8_t id);

/**
 * @brief Gets the current link state of the LAN8670.
 * 
 * The link state can be one of the following:
 * - 0: Link is down
 * - 1: Link is up
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param state Pointer to a variable to store the link state.
 * @note For the LAN8670, the link state bit is always 1 (link is up). If this function returns 0, something weird has happened.
 * @return Status.
 */
int32_t LAN8670_Get_Link_State(lan8670_t *lan, uint8_t *state);

/**
 * @brief Registers HAL ethernet functions for use in this driver.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param ioctx Pointer to the lan8670_IOCtx_t instance containing the HAL functions.
 * @return Status.
 */
int32_t LAN8670_RegisterBusIO(lan8670_t *lan, lan8670_IOCtx_t *ioctx);

/**
 * @brief Reads the PLCA status.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param status Buffer for the returned status value. false=The PLCA reconciliation sublayer is not regularly receiving or transmitting the BEACON, true=The PLCA reconciliation sublayer is regularly receiving or transmitting the BEACON
 * @return Status (an error code, not related to the *status bool).
 */
int32_t LAN8670_PLCA_Get_Status(lan8670_t *lan, bool *status); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Reads the PLCA TOTMR (Transmit Opportunity Timer) setting. Should be 32 bit-times by default.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param buffer Buffer for the register value. The value has a unit of bit-times.
 * @return Status.
 */
int32_t LAN8670_PLCA_ReadTOTMR(lan8670_t *lan, bool *buffer); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Writes the PLCA TOTMR (Transmit Opportunity Timer) setting.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param data Setting to write to the register. Should be an integer with a unit of bit-times.
 * @return Status.
 */
int32_t LAN8670_PLCA_WriteTOTMR(lan8670_t *lan, uint8_t data); // NOTE: This function has not been tested yet (as of 01/31/2026).

/**
 * @brief Returns the value of the PHY_ID1 register. This register contains the first 16 bits of the OUI (Organizationally Unique Identifier). Should be: 0b0000000000000111 in reset according to the datasheet.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param data Buffer for the register value.
 * @return Status.
 */
int32_t LAN8670_Read_PHY_ID1(lan8670_t *lan, uint16_t *data);

/**
 * @brief Returns the PHY manufacturer's model number. Should be 0b00010110 for the LAN8670.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param data Buffer for the value.
 * @return Status.
 */
int32_t LAN8670_Read_Model_Number(lan8670_t *lan, uint8_t *data);

/**
 * @brief Returns the PHY's device address according to the STRAP_CTRL0 register. This is a 5-bit value and should be consistent with how the device address is configured via the hardware pins.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param data Buffer for the value.
 * @return Status.
 */
int32_t LAN8670_Read_PHY_DevAddr(lan8670_t *lan, uint8_t *data);
// clang-format on
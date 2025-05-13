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
typedef int32_t  (*lan8670_Init_Func) (void); 
typedef int32_t  (*lan8670_DeInit_Func) (void);
typedef int32_t  (*lan8670_ReadReg_Func)   (uint32_t, uint32_t, uint32_t *);
typedef int32_t  (*lan8670_WriteReg_Func)  (uint32_t, uint32_t, uint32_t);
typedef int32_t  (*lan8670_GetTick_Func)  (void);

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
 * @param device_address The address of the LAN8670.
 * @param read Function pointer for reading data from the LAN8670.
 * @param write Function pointer for writing data to the LAN8670.
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Init(lan8670_t *lan); // Initializes a LAN8670 instance.

/**
 * @brief Performs a software reset of the LAN8670 Ethernet PHY.
 * @param lan Pointer to the lan8670_t instance.
 * @return 0 on success, or a non-zero error code.
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
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Loopback(lan8670_t *lan, bool setting); // Enables or disables loopback mode on the LAN8670.

/**
 * @brief Enables or disables the LAN8670's low power mode.
 *
 *  When enabled, the PMA will be powered down (meaning the device will not be able to 
 *  send or recieve messages). The remainder of the device will remain functional.
 *
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable low power mode, false to disable it.
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Low_Power_Mode(lan8670_t *lan, bool setting); // Enables or disables the LAN8670's low power mode.

/**
 * @brief Electrically isolates the LAN8670 from MII/RMII.
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to isolate the device, false for normal operation.
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Isolate(lan8670_t *lan, bool setting); // Electrically isolates the LAN8670 from MII/RMII.

/**
 * @brief Enables or disables the LAN8670's collision test mode.
 * 
 *  When enabled, asserting TXEN will cause the COL output to go high
 *  within 512 bit times. Negating TXEN will cause the COL output to go low within 4 bit times. 
 *  This mode should only be enabled when Loopback is enabled.
 *   
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable collision test mode, false to disable it.
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Collision_Test(lan8670_t *lan, bool setting); // Enables or disables the LAN8670's collision test mode.

/**
 * @brief Detects jabber condition on the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param jabber_status Pointer to a boolean variable to store the jabber status.
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Detect_Jabber(lan8670_t *lan, bool *jabber_status); // Detects jabber condition on the LAN8670.

/**
 * @brief Enables or disables collision detection on the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param setting true to enable collision detection, false to disable it.
 * @return 0 on success, or a non-zero error code.
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
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_PLCA_On(lan8670_t *lan, bool setting);

/**
 * @brief Resets the PLCA reconciliation sublayer.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_PLCA_Reset(lan8670_t *lan);

/**
 * @brief Configures the maximum number of nodes supported on the multidrop network.
 *  
 * Proper operation requires that this field be set to at least the 
 * number of nodes that may exist on the network.
 * 
 * @param lan Pointer to the lan8670_t instance.
 * @param node_count The number of nodes on the network.
 * @return 0 on success, or a non-zero error code.
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
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_PLCA_Set_Node_Id(lan8670_t *lan, uint8_t id);

/**
 * @brief Gets the current link state of the LAN8670.
 * @param lan Pointer to the lan8670_t instance.
 * @param link_up Pointer to a boolean variable to store the link state (true if link is up, false if down).
 * @return 0 on success, or a non-zero error code.
 */
int32_t LAN8670_Get_Link_State(lan8670_t *lan, bool *link_up);

// clang-format on
#ifndef _U_NX_ETHERNET_H
#define _U_NX_ETHERNET_H

// clang-format off

/*
*   NOTE: This file can only be used in projects that include NetXDuo.
*/

#include <stdint.h>
#include <stdbool.h>
#include "nx_api.h"

/* CONFIG */
#define ETH_UDP_PORT	    2006 /* UDP port for communication */
#define ETH_MESSAGE_SIZE    8    /* Maximum ethernet message size in bytes. */
#define ETH_MAX_PACKETS     10   /* Maximum number of packets we wanna handle simultaneously */
#define ETH_NUMBER_OF_NODES 8    /* Number of nodes in the network. */

typedef enum {
	TPU = (1 << 0),     // 0b00000001
	VCU = (1 << 1), 	// 0b00000010
	COMPUTE = (1 << 2), // 0b00000100
	MSB1 = (1 << 3),    // 0b00001000
	MSB2 = (1 << 4),    // 0b00010000
	MSB3 = (1 << 5),    // 0b00100000
	MSB4 = (1 << 6),    // 0b01000000
	NODE8 = (1 << 7),   // 0b10000000
} ethernet_node_t;
#define ETH_IP(node) IP_ADDRESS(224, 0, 0, node)

/* These node ids are ONLY relavent to PLCA configuration.
   They are meant to be used when configuring a PHY. The IDs must be sequential, and the "0" id always indicates the network's coordinator node.
   They have no impact on application-level IP addresses or message processing. 
   
   For example, if you're using the LAN8670 PHY driver, you'd probably use these enum values like this:
   LAN8670_PLCA_Set_Node_Count(&lan8670, PLCA_NUM_NODES);
   LAN8670_PLCA_Set_Node_Id(&lan8670, PLCA_VCU) // replace 'PLCA_VCU' with whatever board it is		
   */
typedef enum {
	PLCA_TPU,		// 0. This is the PLCA coordinator node.
	PLCA_VCU,
	PLCA_COMPUTE,
	PLCA_MSB1,
	PLCA_MSB2,
	PLCA_MSB3,
	PLCA_MSB4,
	PLCA_NODE8,
	PLCA_NUM_NODES
} plca_node_id_t;
/* END CONFIG */

typedef struct {
	uint8_t sender_id;
	uint8_t recipient_id;
	uint8_t message_id;
	uint8_t data_length;
	uint8_t data[10]; // change back
} ethernet_message_t;

/* Function Pointers (for initialization). */
typedef void (*DriverFunction)(NX_IP_DRIVER *); /* User-supplied network driver used to send and receive IP packets. */
typedef void (*OnRecieve)(ethernet_message_t message); /* User-supplied function that will be called whenever an ethernet message is recieved. */

/**
 * @brief Initializes the NetX ethernet system in a repo.
 * @param node_id The ID (ethernet_node_t) of this node.
 * @param driver User-supplied network driver function. Should be set to nx_stm32_eth_driver (from "nx_stm32_eth_driver.h") for STM32 projects.
 * @param on_recieve User-supplied function to be called whenever an ethernet message is recieved. The function's only parameter is an ethernet_message_t instance containing the recieved message.
 * @return Status.
 */
uint8_t ethernet_init(ethernet_node_t node_id, DriverFunction driver, OnRecieve on_recieve);

/**
 * @brief Creates an ethernet message. Can be send with ethernet_send_message(), or added to a queue.
 * @param recipient_id The ID of the recipient node.
 * @param message_id The ID of the message.
 * @param data Pointer to the data to include in the message.
 * @param data_length Length of the data in bytes.
 * @return The created ethernet message.
 */
ethernet_message_t ethernet_create_message(uint8_t message_id, ethernet_node_t recipient_id, uint8_t *data, uint8_t data_length);

/**
 * @brief Sends an ethernet message.
 * @param message The message to send.
 * @return Status.
 */
uint8_t ethernet_send_message(ethernet_message_t *message);

// clang-format on
#endif /* u_nx_ethernet.h */
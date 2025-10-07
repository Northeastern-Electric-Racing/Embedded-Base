// clang-format off
#include "u_nx_ethernet.h"
#include "u_nx_debug.h"
#include "u_tx_debug.h"
#include "nx_api.h"
#include <string.h>
#include <stdio.h>

/* PRIVATE MACROS */
#define _PACKET_POOL_SIZE \
	((sizeof(ethernet_message_t) + sizeof(NX_PACKET)) * ETH_MAX_PACKETS)
#define _IP_THREAD_STACK_SIZE 2048
#define _ARP_CACHE_SIZE	      1024
#define _IP_THREAD_PRIORITY   1
#define _IP_NETWORK_MASK      IP_ADDRESS(255, 255, 255, 0)
#define _UDP_QUEUE_MAXIMUM    12

/* DEVICE INFO */
typedef struct {
	/* NetX Objects */
	NX_UDP_SOCKET socket;
	NX_PACKET_POOL packet_pool;
	NX_IP ip;

	/* Static memory for NetX stuff */
	UCHAR packet_pool_memory[_PACKET_POOL_SIZE];
	UCHAR ip_memory[_IP_THREAD_STACK_SIZE];
	UCHAR arp_cache_memory[_ARP_CACHE_SIZE];

	/* Device config variables */
	bool is_initialized;
	uint8_t node_id;
	DriverFunction
		driver; /* Set by the user. Used to communicate with the driver layer. */
	OnRecieve on_recieve; /* Set by the user. Called when a message is recieved. */
} _ethernet_device_t;
static _ethernet_device_t device = { 0 };

/* Callback function. Called when an ethernet message is received. */
static void _receive_message(NX_UDP_SOCKET *socket) {
    NX_PACKET *packet;
    ULONG bytes_copied;
    uint8_t status;
    ethernet_message_t message = {0};

    /* Recieve the packet */
    status = nx_udp_socket_receive(socket, &packet, NX_NO_WAIT);
    if(status == NX_SUCCESS) {
        /* Extract message from packet */
        status = nx_packet_data_extract_offset(
            packet,                         // Packet to extract from
            0,                              // Offset (start of packet)
            &message,                       // Message buffer
            sizeof(ethernet_message_t),     // Size to extract
            &bytes_copied                   // Stores how many bytes were actually copied to &message
        );
        if(bytes_copied < sizeof(ethernet_message_t)) {
            DEBUG_PRINTLN("WARNING: Received ethernet message was smaller than expected (only received %lu of %u expected bytes).", bytes_copied, sizeof(ethernet_message_t));
        }

        /* Process received message */
        if(status == NX_SUCCESS) {
            DEBUG_PRINTLN("Received ethernet message! (Sender ID: %d, Message ID: %d).", message.sender_id, message.message_id);
            device.on_recieve(message);
        }
    }

    /* Release the packet */
    nx_packet_release(packet);
}

/* API FUNCTIONS */

uint8_t ethernet_init(ethernet_node_t node_id, DriverFunction driver, OnRecieve on_recieve) {
    
    uint8_t status;

    /* Make sure device isn't already initialized */
    if(device.is_initialized) {
        DEBUG_PRINTLN("ERROR: Ethernet is already initialized.");
        return U_ERROR;
    }

    /* Store node id and user-supplied function pointers. */
    device.node_id = node_id;
    device.driver = driver;
    device.on_recieve = on_recieve;

    /* Create packet pool */
    status = nx_packet_pool_create(
        &device.packet_pool,        // Pointer to the packet pool instance
        "Ethernet Packet Pool",     // Name
        sizeof(ethernet_message_t), // Payload size (i.e. the size of each packet)
        device.packet_pool_memory,  // Pointer to the pool's memory area
        _PACKET_POOL_SIZE           // Size of the pool's memory area
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to create packet pool (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Create IP instance */
    status = nx_ip_create(
        &device.ip,                          // Pointer to the IP instance
        "Ethernet IP Instance",              // Name
        IP_ADDRESS(5, 5, 5, device.node_id), // Dummy unicast IP (we shouldn't have to use this if we're just using multicast for everything)
        _IP_NETWORK_MASK,                    // Network mask
        &device.packet_pool,                 // Pointer to the packet pool
        device.driver,                       // Pointer to the Ethernet driver function
        device.ip_memory,                    // Pointer to the memory for the IP instance
        _IP_THREAD_STACK_SIZE,               // Size of the IP thread stack
        _IP_THREAD_PRIORITY                  // Priority of the IP thread
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to create IP instance (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Enable ARP */
    status = nx_arp_enable(
        &device.ip,                // IP instance
        device.arp_cache_memory,   // Memory for ARP cache
        _ARP_CACHE_SIZE            // Size of ARP cache
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to enable ARP (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }


    /* Enable UDP */
    status = nx_udp_enable(&device.ip);
    if (status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to enable UDP (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Enable igmp */
    status = nx_igmp_enable(&device.ip);
    if (status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to enable igmp (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Set up multicast groups. 
    *  (This iterates through every possible node combination between 0b00000001 and 0b11111111.
    *  If any of the combinations include device.node_id, that combination gets added as a multicast group.
    *  This ensures that ethernet messages can be sent to all possible combinations of recipients.)
    * 
    *  Note: This is probably pretty inefficient. I did it so you don't have to manually set up
    *        multicast groups any time you want to send a message to a new combination of nodes,
    *        but if this setup ends up being too slow or something, feel free to get rid of it.
    */
    for(int i = (1 << 0); i < (1 << 8); i++) {
        if(i & device.node_id) {
            ULONG address = ETH_IP(i);
            status = nx_igmp_multicast_join(&device.ip, address);
            if(status != NX_SUCCESS) {
                DEBUG_PRINTLN("ERROR: Failed to join multicast group (Status: %d/%s, Address: %lu).", status, nx_status_toString(status), address);
            }
        }
    }

    /* Create UDP socket for broadcasting */
    status = nx_udp_socket_create(
        &device.ip,                 // IP instance
        &device.socket,             // Socket to create
        "Ethernet Broadcast",       // Socket name
        NX_IP_NORMAL,               // Type of service
        NX_FRAGMENT_OKAY,           // Fragment flag
        NX_IP_TIME_TO_LIVE,         // Time to live
        _UDP_QUEUE_MAXIMUM          // UDP queue maximum
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to create UDP socket (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Bind socket to broadcast port */
    status = nx_udp_socket_bind(
        &device.socket,              // Socket to bind
        ETH_UDP_PORT,                // Port
        TX_WAIT_FOREVER              // Wait forever
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to bind UDP socket (Status: %d/%s).", status, nx_status_toString(status));
        nx_udp_socket_delete(&device.socket);
        return status;
    }

    /* Enable UDP recieve callback */
    status = nx_udp_socket_receive_notify(
        &device.socket,               // Socket to set callback for
        &_receive_message             // Callback function
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to set recieve callback (Status: %d/%s).", status, nx_status_toString(status));
        nx_udp_socket_unbind(&device.socket);
        nx_udp_socket_delete(&device.socket);
        return status;
    }

    /* Mark device as initialized. */
    device.is_initialized = true;

    DEBUG_PRINTLN("Ran ethernet_init().");
    return NX_SUCCESS;
}

/* Creates an ethernet message (i.e. returns an ethernet_message_t instance). */
ethernet_message_t ethernet_create_message(uint8_t message_id, ethernet_node_t recipient_id, uint8_t *data, uint8_t data_length) {
    ethernet_message_t message = {0};

    /* Check data length */
    if (data_length > ETH_MESSAGE_SIZE) {
        DEBUG_PRINTLN("ERROR: Data length exceeds maximum (message_id: %d).", message_id);
        return message; // Return empty message.
    }

    /* Prepare message */
    message.sender_id = device.node_id;
    message.recipient_id = recipient_id;
    message.message_id = message_id;
    message.data_length = data_length;
    memcpy(message.data, data, data_length);

    return message;
}

/* Sends an ethernet message (i.e. sends a UDP packet). */
uint8_t ethernet_send_message(ethernet_message_t *message) {
    NX_PACKET *packet;
    uint8_t status;

    /* Check if ethernet is initialized */
    if(!device.is_initialized) {
        DEBUG_PRINTLN("ERROR: Ethernet device is not initialized, so ethernet_send_message() will not work.");
        return U_ERROR;
    }

    /* Check data length */
    if (message->data_length > ETH_MESSAGE_SIZE) {
        DEBUG_PRINTLN("ERROR: Data length exceeds maximum (Message ID: %d).", message->message_id);
        return U_ERROR;
    }

    /* Allocate a packet */
    status = nx_packet_allocate(
        &device.packet_pool,        // Packet pool
        &packet,                    // Packet
        NX_UDP_PACKET,              // Packet type
        TX_WAIT_FOREVER             // Wait indefinitely until a packet is available
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to allocate packet (Status: %d/%s, Message ID: %d).", status, nx_status_toString(status), message->message_id);
        return U_ERROR;
    }

    /* Append message data to packet */
    status = nx_packet_data_append(
        packet,                     // Packet
        &message,                   // Data to append
        sizeof(ethernet_message_t), // Size of data
        &device.packet_pool,        // Packet pool
        TX_WAIT_FOREVER             // Wait indefinitely
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to append data to packet (Status: %d/%s, Message ID: %d).", status, nx_status_toString(status), message->message_id);
        nx_packet_release(packet);
        return U_ERROR;
    }

    /* Send message */
    status = nx_udp_socket_send(
        &device.socket,
        packet,
        ETH_IP(message->recipient_id),
        ETH_UDP_PORT
    );
    if(status != NX_SUCCESS) {
        DEBUG_PRINTLN("ERROR: Failed to send packet (Status: %d/%s, Message ID: %d).", status, nx_status_toString(status), message->message_id);
        nx_packet_release(packet);
        return U_ERROR;
    }

    DEBUG_PRINTLN("Sent ethernet message (Recipient ID: %d, Message ID: %d).", message->recipient_id, message->message_id);
    return U_SUCCESS;
}
// clang-format on
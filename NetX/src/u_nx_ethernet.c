// clang-format off
#include "u_nx_ethernet.h"
#include "nx_stm32_eth_driver.h"
#include "nxd_ptp_client.h"
#include "tx_api.h"
#include "u_nx_debug.h"
#include "u_tx_debug.h"
#include "c_utils.h"
#include "nx_api.h"
#include "u_tx_general.h"
#include <string.h>
#include <stdio.h>
#include <sys/_types.h>
#if ETH_ENABLE_MQTT
#include "nxd_mqtt_client.h"
#endif

/* PRIVATE MACROS */
#define _IP_THREAD_STACK_SIZE 4096
#define _ARP_CACHE_SIZE	      2048
#define _IP_THREAD_PRIORITY   1
#define _IP_NETWORK_MASK      IP_ADDRESS(255, 255, 255, 0)
#define _UDP_QUEUE_MAXIMUM    12
#define _PTP_THREAD_PRIORITY 2
#define _MQTT_THREAD_PRIORITY 2

/* The DEFAULT_PAYLOAD_SIZE should match with RxBuffLen configured via MX_ETH_Init */
#define DEFAULT_PAYLOAD_SIZE      1524
#define NX_APP_PACKET_POOL_SIZE              ((DEFAULT_PAYLOAD_SIZE + sizeof(NX_PACKET)) * 100)
#define MQTT_CLIENT_STACK_SIZE 8192

extern ETH_HandleTypeDef heth;

/* DEVICE INFO */
typedef struct {
   	uint8_t node_id;

	NX_IP ip;
	UCHAR ip_memory[_IP_THREAD_STACK_SIZE];
	DriverFunction
		driver; /* Set by the user. Used to communicate with the driver layer. */
	OnRecieve on_recieve; /* Set by the user. Called when a message is recieved. */

	NX_PACKET_POOL packet_pool;
	UCHAR packet_pool_memory[NX_APP_PACKET_POOL_SIZE];

	UCHAR arp_cache_memory[_ARP_CACHE_SIZE];

	#if ETH_ENABLE_MANUAL_UDP_MULTICAST
	NX_UDP_SOCKET socket;
	#endif

	#if ETH_ENABLE_MQTT
	NXD_MQTT_CLIENT mqtt_client;
	UCHAR mqtt_thread_stack[MQTT_CLIENT_STACK_SIZE / sizeof(ULONG)];
	#endif


	NX_PTP_CLIENT ptp_client;
	SHORT ptp_utc_offset;
	ULONG ptp_stack[2048 / sizeof(ULONG)];

	bool is_initialized;
} _ethernet_device_t;
static _ethernet_device_t device = { 0 };

/* CALLBACK FUNCTIONS */
#if ETH_ENABLE_MQTT
static VOID _mqtt_disconnect_callback(NXD_MQTT_CLIENT *client_ptr)
{
    PRINTLN_WARNING("client disconnected from server\n");
}

static VOID _mqtt_recieve_callback(NXD_MQTT_CLIENT* client_ptr, UINT number_of_messages)
{
    //tx_event_flags_set(&mqtt_app_flag, DEMO_MESSAGE_EVENT, TX_OR);
    return;
}
#endif


/* Callback function. Called when a PTP event is processed. */
// extern UINT ptp_clock_callback(NX_PTP_CLIENT *client_ptr, UINT operation,
//         NX_PTP_TIME *time_ptr, NX_PACKET *packet_ptr,
//         VOID *callback_data);

/* Callback function. Called when a PTP event is processed. */
static UINT _ptp_event_callback(NX_PTP_CLIENT *ptp_client_ptr, UINT event, VOID *event_data, VOID *callback_data)
{
  NX_PARAMETER_NOT_USED(callback_data);

  switch (event)
  {
    case NX_PTP_CLIENT_EVENT_MASTER:
    {
      PRINTLN_WARNING("new MASTER clock!");
      break;
    }

    case NX_PTP_CLIENT_EVENT_SYNC:
    {
      nx_ptp_client_sync_info_get((NX_PTP_CLIENT_SYNC *)event_data, NX_NULL, &device.ptp_utc_offset);
      PRINTLN_INFO("SYNC event: utc offset=%d", device.ptp_utc_offset);
      break;
    }

    case NX_PTP_CLIENT_EVENT_TIMEOUT:
    {
      PRINTLN_WARNING("Master clock TIMEOUT!");
      break;
    }
    default:
    {
      break;
    }
  }

  return 0;
}

/* Callback function. Called when an ethernet message is received. */
#if ETH_ENABLE_MANUAL_UDP_MULTICAST
static void _receive_message(NX_UDP_SOCKET *socket) {
    NX_PACKET *packet;
    ULONG bytes_copied;
    uint8_t status;
    ethernet_message_t message = {0};

    /* Recieve the packet */
    status = nx_udp_socket_receive(socket, &packet, NX_NO_WAIT);
    if(status == NX_SUCCESS) {
        PRINTLN_INFO("Recieved UDP socket!");

        /* Get packet information (for debugging). */
        ULONG ip_address;
        UINT protocol;
        UINT port;
        UINT interface_index;
        status = nx_udp_packet_info_extract(packet, &ip_address, &protocol, &port, &interface_index);
        if(status != NX_SUCCESS) {
            PRINTLN_WARNING("Failed to extract UDP packet information (Status: %d/%s).", status, nx_status_toString(status));
        }
        else {
            uint8_t ip_address_byte1 = (ip_address >> 24) & 0xFF;
            uint8_t ip_address_byte2 = (ip_address >> 16) & 0xFF;
            uint8_t ip_address_byte3 = (ip_address >> 8) & 0xFF;
            uint8_t ip_address_byte4 = ip_address & 0xFF;
            PRINTLN_INFO("UDP Packet - IP: %d.%d.%d.%d, Port: %d, Protocol: %d, Interface: %d", ip_address_byte1, ip_address_byte2, ip_address_byte3, ip_address_byte4, port, protocol, interface_index);
        }

        /* Extract message from packet */
        ULONG bytes_copied = 0;
        status = nx_packet_data_retrieve(packet, &message, &bytes_copied);
        if(status != NX_SUCCESS) {
            PRINTLN_ERROR("Failed to call nx_packet_data_retrieve() (Status: %d/%s).", status, nx_status_toString(status));
        }

        /* Process received message */
        if(status == NX_SUCCESS) {
            PRINTLN_INFO("Received ethernet message! (Sender ID: %d, Message ID: %d, bytes_copied: %d).", message.sender_id, message.message_id, bytes_copied);
            device.on_recieve(message);
        }
    }

    /* Release the packet */
    nx_packet_release(packet);
}
#endif

/* API FUNCTIONS */

UINT ethernet_init(ethernet_node_t node_id, DriverFunction driver, OnRecieve on_recieve) {

    UINT status;
    device.ptp_utc_offset = 0; // no offset to start

    /* Make sure device isn't already initialized */
    if(device.is_initialized) {
        PRINTLN_ERROR("Ethernet is already initialized.");
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
        DEFAULT_PAYLOAD_SIZE, // Payload size (i.e. the size of each packet)
        device.packet_pool_memory,  // Pointer to the pool's memory area
        NX_APP_PACKET_POOL_SIZE           // Size of the pool's memory area
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to create packet pool (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Create IP instance */
    status = nx_ip_create(
        &device.ip,                          // Pointer to the IP instance
        "Ethernet IP Instance",              // Name
        IP_ADDRESS(10, 0, 0, device.node_id), // Unicast IP derived from node_id, 10.0.0.0/24
        _IP_NETWORK_MASK,                    // Network mask /24
        &device.packet_pool,                 // Pointer to the packet pool
        device.driver,                       // Pointer to the Ethernet driver function
        device.ip_memory,                    // Pointer to the memory for the IP instance
        _IP_THREAD_STACK_SIZE,               // Size of the IP thread stack
        _IP_THREAD_PRIORITY                  // Priority of the IP thread
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to create IP instance (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    // behold the magic
    // this is a poll function to wait until the driver has completely initialized
    // without it the arp module and the TCP module fail silently and confusingly
    ULONG current_status;
    do {
        nx_ip_driver_direct_command(&device.ip, 51, &current_status);
        tx_thread_sleep(100);
    } while (current_status != 4);// NX_DRIVER_STATE_LINK_ENABLED

    /* Enable ARP */
    status = nx_arp_enable(
        &device.ip,                // IP instance
        device.arp_cache_memory,   // Memory for ARP cache
        _ARP_CACHE_SIZE            // Size of ARP cache
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to enable ARP (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }


    /* Enable igmp */
#if ETH_ENABLE_IGMP || ETH_ENABLE_MANUAL_UDP_MULTICAST
    status = nx_igmp_enable(&device.ip);
    if (status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to enable igmp (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }
#endif

    status = nx_icmp_enable(&device.ip);
    if (status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to enable icmp (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Enable UDP */
    status = nx_udp_enable(&device.ip);
    if (status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to enable UDP (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    status = nx_tcp_enable(&device.ip);
    if (status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to enable TCP (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

#if ETH_ENABLE_MANUAL_UDP_MULTICAST
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
                PRINTLN_ERROR("Failed to join multicast group (Status: %d/%s, Address: %lu).", status, nx_status_toString(status), address);
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
        PRINTLN_ERROR("Failed to create UDP socket (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Bind socket to broadcast port */
    status = nx_udp_socket_bind(
        &device.socket,              // Socket to bind
        ETH_UDP_PORT,                // Port
        TX_WAIT_FOREVER              // Wait forever
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to bind UDP socket (Status: %d/%s).", status, nx_status_toString(status));
        nx_udp_socket_delete(&device.socket);
        return status;
    }

    /* Enable UDP recieve callback */
    status = nx_udp_socket_receive_notify(
        &device.socket,               // Socket to set callback for
        &_receive_message             // Callback function
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to set recieve callback (Status: %d/%s).", status, nx_status_toString(status));
        nx_udp_socket_unbind(&device.socket);
        nx_udp_socket_delete(&device.socket);
        return status;
    }
#endif

    /* Create the PTP client instance */
    status = nx_ptp_client_create(&device.ptp_client, &device.ip, 0, &device.packet_pool,
                           _PTP_THREAD_PRIORITY, (UCHAR *)&device.ptp_stack, sizeof(device.ptp_stack),
                           _nx_ptp_client_soft_clock_callback, NX_NULL);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to create PTP client (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* start the PTP client */
    status = nx_ptp_client_start(&device.ptp_client, NX_NULL, 0, 0, 0, _ptp_event_callback, NX_NULL);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to start PTP client (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

#if ETH_ENABLE_MQTT
/* Create MQTT client instance. */
    char client_id[8] = "";
    UINT client_id_size = sprintf(client_id, "FWD-%d", device.node_id);

    status = nxd_mqtt_client_create(&device.mqtt_client, "MQTT client",
        client_id, client_id_size, &device.ip, &device.packet_pool,
        (VOID*)device.mqtt_thread_stack, sizeof(device.mqtt_thread_stack),
        _MQTT_THREAD_PRIORITY, NX_NULL, 0);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to create MQTT client (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Register the disconnect notification function. */
    status = nxd_mqtt_client_disconnect_notify_set(&device.mqtt_client, _mqtt_disconnect_callback);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to create MQTT disconnect notification (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Set the receive notify function. */
    status = nxd_mqtt_client_receive_notify_set(&device.mqtt_client, _mqtt_recieve_callback);
    if (status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to set mqtt recv notification (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    NXD_ADDRESS server_ip;
    server_ip.nxd_ip_version = 4;
    server_ip.nxd_ip_address.v4 = ETH_MQTT_SERVER_IP;
    /* Start the connection to the server. */
    status = nxd_mqtt_client_connect(&device.mqtt_client, &server_ip, ETH_MQTT_SERVER_PORT,
        1000, NX_TRUE, NX_WAIT_FOREVER);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to connect to MQTT client (Status: %d/%s).", status, nx_status_toString(status));
    } else {
    }
#endif
    /* Mark device as initialized. */
    device.is_initialized = true;

    PRINTLN_INFO("Ran ethernet_init()");
    return NX_SUCCESS;
}

/* Creates an ethernet message (i.e. returns an ethernet_message_t instance). */
#if ETH_ENABLE_MANUAL_UDP_MULTICAST
ethernet_message_t ethernet_create_message(uint8_t message_id, ethernet_node_t recipient_id, uint8_t *data, uint8_t data_length) {
    ethernet_message_t message = {0};

    /* Check data length */
    if (data_length > ETH_MESSAGE_SIZE) {
        PRINTLN_ERROR("Data length exceeds maximum (message_id: %d).", message_id);
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
        PRINTLN_ERROR("Ethernet device is not initialized, so ethernet_send_message() will not work.");
        return U_ERROR;
    }

    /* Check data length */
    if (message->data_length > ETH_MESSAGE_SIZE) {
        PRINTLN_ERROR("Data length exceeds maximum (Message ID: %d).", message->message_id);
        return U_ERROR;
    }

    PRINTLN_INFO("got to this part of ethernet_send_message()");

    /* Make sure interface is up. */
    ULONG actual_status = 0;
    status = nx_ip_interface_status_check(&device.ip, 0, NX_IP_LINK_ENABLED, &actual_status, 1000);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to call nx_ip_interface_status_check() (Status: %d/%s).", status, nx_status_toString(status));
        return U_ERROR;
    } else {
        PRINTLN_INFO("Succeeded calling nx_ip_interface_status_check() (Status: %d/%s).", status, nx_status_toString(status));
    }

    /* Allocate a packet */
    status = nx_packet_allocate(
        &device.packet_pool,        // Packet pool
        &packet,                    // Packet
        NX_UDP_PACKET,              // Packet type
        TX_WAIT_FOREVER             // Wait indefinitely until a packet is available
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to allocate packet (Status: %d/%s, Message ID: %d).", status, nx_status_toString(status), message->message_id);
        return U_ERROR;
    }

    PRINTLN_INFO("got to nx_packet_allocate() part of send message");

    /* Append message data to packet */
    status = nx_packet_data_append(
        packet,                     // Packet
        message,                    // Data to append
        sizeof(ethernet_message_t), // Size of data
        &device.packet_pool,        // Packet pool
        TX_WAIT_FOREVER             // Wait indefinitely
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to append data to packet (Status: %d/%s, Message ID: %d).", status, nx_status_toString(status), message->message_id);
        nx_packet_release(packet);
        return U_ERROR;
    }

    PRINTLN_INFO("got to nx_packet_data_append() part of send message");

    ULONG length = 0;
    status = nx_packet_length_get(packet, &length);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to call nx_packet_length_get() (Status: %d/%n).", status, nx_status_toString(status));
    } else {
        PRINTLN_INFO("Packet has length of %d!", length);
    }

    /* Send message */
    status = nx_udp_socket_send(
        &device.socket,
        packet,
        ETH_IP(message->recipient_id),
        ETH_UDP_PORT
    );
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to send packet (Status: %d/%s, Message ID: %d, Recipient ID: %d, IP Address: %d, IP components: 224.0.0.%d).", status, nx_status_toString(status), message->message_id, message->recipient_id, ETH_IP(message->recipient_id), ETH_IP(message->recipient_id));
        nx_packet_release(packet);
        return U_ERROR;
    }

    PRINTLN_INFO("got to nx_udp_socket_send() part of send message");

    PRINTLN_INFO("Sent ethernet message (Recipient ID: %d, Message ID: %d, Message Contents: %d).", message->recipient_id, message->message_id, message->data);
    return U_SUCCESS;
}
#endif

#if ETH_ENABLE_MQTT
UINT ethernet_mqtt_publish(char *topic_name, UINT topic_size, char *message, UINT message_size) {
    return nxd_mqtt_client_publish(&device.mqtt_client, topic_name, topic_size-1, message, message_size, NX_FALSE, 0, MS_TO_TICKS(100));
}

UINT ethernet_mqtt_reconnect(void) {
    NXD_ADDRESS server_ip;
    server_ip.nxd_ip_version = 4;
    server_ip.nxd_ip_address.v4 = ETH_MQTT_SERVER_IP;
    // TODO fix bug that breaks reconnection with 0x10007
    nxd_mqtt_client_delete(&device.mqtt_client);

    char client_id[8] = "";
    UINT client_id_size = sprintf(client_id, "FWD-%d", device.node_id);

    UINT status = nxd_mqtt_client_create(&device.mqtt_client, "MQTT client",
        client_id, client_id_size, &device.ip, &device.packet_pool,
        (VOID*)device.mqtt_thread_stack, sizeof(device.mqtt_thread_stack),
        _MQTT_THREAD_PRIORITY, NX_NULL, 0);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to create MQTT client (Status: %d/%s).", status, nx_status_toString(status));
        return status;
    }

    /* Start the connection to the server. */
    return nxd_mqtt_client_connect(&device.mqtt_client, &server_ip, ETH_MQTT_SERVER_PORT,
        1000, NX_TRUE, NX_WAIT_FOREVER);
}
#endif

NX_PTP_DATE_TIME ethernet_get_time(void) {
    NX_PTP_TIME tm = { 0 };
    NX_PTP_DATE_TIME date = { 0 };

    /* If not initialized, don't try to read PTP yet. */
    if(!device.is_initialized) {
        PRINTLN_ERROR("Tried getting PTP time before device has been initialized.");
        return date;
    }

    /* read the PTP clock */
    nx_ptp_client_time_get(&device.ptp_client, &tm);

    /* convert PTP time to UTC date and time */
    nx_ptp_client_utility_convert_time_to_date(&tm, 0, &date);

    return date;
}

UINT ethernet_print_arp_status(void) {
    ULONG arp_requests_sent = 100;
    ULONG arp_requests_received = 100;

    ULONG arp_responses_sent = 100;
    ULONG arp_responses_received;
                            ULONG arp_dynamic_entries= 100;
                            ULONG arp_static_entries= 100;
                            ULONG arp_aged_entries= 100;
                            ULONG arp_invalid_messages= 100;
    UINT status = nx_arp_info_get(&device.ip,  &arp_requests_sent, &arp_requests_received,
                             &arp_responses_sent,  &arp_responses_received,
                             &arp_dynamic_entries,  &arp_static_entries,
                             &arp_aged_entries,  &arp_invalid_messages);
    if(status != NX_SUCCESS) {
        PRINTLN_ERROR("Failed to retrieve ARP info (Status: %d/%s)", status, nx_status_toString(status));
        return status;
    }
    PRINTLN_INFO("ARP info REQ SENT %lu, REQ RECV %lu, RESP SENT %lu, RESP RECV %lu, DYN ENTRY %lu, S ENTRY %lu, AGED %lu, INVALID %lu",  arp_requests_sent,  arp_requests_received,
                             arp_responses_sent,  arp_responses_received,
                             arp_dynamic_entries,  arp_static_entries,
                             arp_aged_entries,  arp_invalid_messages);

    return status;
}


// clang-format on

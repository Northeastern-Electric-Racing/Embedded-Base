#include <string.h>
#include "u_tx_debug.h"
#include "u_nx_protobuf.h"
#include "u_nx_ethernet.h"

ethernet_mqtt_message_t _nx_protobuf_create_mqtt_message(const char* topic, const char* unit, float[] values, int values_size) {
    /* Zero-initialize the protobuf struct and the sendable ethernet_mqtt_message_t message. */
    serverdata_v2_ServerData protobuf =  serverdata_v2_ServerData_init_zero;
    ethernet_mqtt_message_t message = { 0 };
    message.initialized = false;

    /* Calculate topic size. */
    int topic_size = strlen(topic);
    if(topic_size > ETH_MAX_TOPIC_SIZE) {
        PRINTLN_ERROR("MQTT Message topic exceeds maximum length (Topic: %s).", topic);
        return message; // Return empty, uninitialized message.
    }

    /* Get the PTP time and convert to appropriate protobuf time. */
    NX_PTP_DATE_TIME datetime = ethernet_get_time();
    uint64_t time_us = datetime.nanosecond; // u_TODO - this is temporary
    // u_TODO - actually figure out how to convert this like it should be. Probably ask jack how this should be set up?

    /* Pack the protobuf message. */
    // PROTOBUF SCHEMA LOOKS LIKE THIS:
    // typedef struct _serverdata_v2_ServerData {
    //     char unit[15];
    //     uint64_t time_us;
    //     pb_size_t values_count;
    //     float values[5];
    // } serverdata_v2_ServerData;
    *(msg.unit) = 
}
#include <string.h>
#include "u_tx_debug.h"
#include "u_nx_protobuf.h"
#include "u_nx_ethernet.h"
#include "pb.h"
#include "pb_encode.h"
#include "tx_api.h"

ethernet_mqtt_message_t _nx_protobuf_mqtt_message_create(const char* topic, size_t topic_len, const char* unit,  size_t unit_len, const float values[], int values_size) {
    /* Zero-initialize the protobuf struct and the sendable ethernet_mqtt_message_t message. */
    serverdata_v2_ServerData protobuf =  serverdata_v2_ServerData_init_zero;
    ethernet_mqtt_message_t message = { 0 };
    message.initialized = false;

    /* Enforce topic length. */
    if(topic_len > PB_MAX_TOPIC_LENGTH) {
        PRINTLN_ERROR("MQTT Message topic exceeds maximum length of %d (Topic: %s, Current Topic Length: %d).", PB_MAX_TOPIC_LENGTH, topic, topic_len);
        return message; // Return empty, uninitialized message.
    }
    // NOTE: If using the `nx_protobuf_mqtt_message_create()` macro (as intended), the static asserts should catch this. This is just an extra check in case a caller uses this function directly for whatever reason.

    /* Enforce unit length. */
    if(unit_len > PB_MAX_UNIT_LENGTH) {
        PRINTLN_ERROR("MQTT Unit string length exceeds maximum length of %d (Topic: %s, Current Unit String Length: %d).", PB_MAX_UNIT_LENGTH, topic, unit_len);
        return message; // Return empty, uninitialized message.
    }
    // NOTE: If using the `nx_protobuf_mqtt_message_create()` macro (as intended), the static asserts should catch this. This is just an extra check in case a caller uses this function directly for whatever reason.

    /* Enforce minimum number of datapoints. */
    if(values_size < PB_MIN_DATAPOINTS) {
        PRINTLN_ERROR("Message must have at least %d datapoints (Topic: %s, Current values_size: %d).", PB_MIN_DATAPOINTS, topic, values_size);
        return message; // Return empty, uninitialized message.
    }
    // NOTE: If using the `nx_protobuf_mqtt_message_create()` macro (as intended), the static asserts should catch this. This is just an extra check in case a caller uses this function directly for whatever reason.

    /* Enforce maximum number of datapoints. */
    if(values_size > PB_MAX_DATAPOINTS) {
        PRINTLN_ERROR("Message cannot have more than %d datapoints (Topic: %s, Current values_size: %d).", PB_MAX_DATAPOINTS, topic, values_size);
        return message; // Return empty, uninitialized message.
    }
    // NOTE: If using the `nx_protobuf_mqtt_message_create()` macro (as intended), the static asserts should catch this. This is just an extra check in case a caller uses this function directly for whatever reason.

    /* Get the PTP time and convert to appropriate protobuf time. */
    NX_PTP_DATE_TIME datetime = ethernet_get_time();
    uint64_t time_us = datetime.nanosecond; // u_TODO - this is temporary
    // u_TODO - actually figure out how to convert this like it should be. Probably ask jack how this should be set up?

    /* Pack the protobuf message. */
    // CURRENT PROTOBUF SCHEMA LOOKS LIKE THIS:
    // typedef struct _serverdata_v2_ServerData {
    //     char unit[15];
    //     uint64_t time_us;
    //     pb_size_t values_count;
    //     float values[5];
    // } serverdata_v2_ServerData;
    memcpy(protobuf.unit, unit, unit_len);
    protobuf.time_us = time_us;
    protobuf.values_count = values_size;
    memcpy(protobuf.values, values, values_size * sizeof(float));

    /* Pack the `ethernet_mqtt_message_t` object and return it as successfully initialized. */
    message.topic = topic;
    message.topic_size = topic_len;
    message.protobuf = protobuf;
    message.initialized = true;
    return message;
}

int nx_protobuf_mqtt_message_send(ethernet_mqtt_message_t* message) {
    /* Make sure message isn't nullptr. */
    if(!message) {
        PRINTLN_ERROR("Null pointer to `ethernet_mqtt_message_t` message.");
        return U_ERROR;
    }

    /* Make sure message is initialized. */
    if(!message->initialized) {
        PRINTLN_ERROR("Attempting to send an uninitialized `ethernet_mqtt_message_t` message.");
        return U_ERROR;
    }

    /* Set up the buffer. */
    unsigned char buffer[serverdata_v2_ServerData_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    /* Encode the protobuf. */
    int status = pb_encode(&stream, serverdata_v2_ServerData_fields, &message->protobuf);
    if(status != true) {
        PRINTLN_ERROR("Failed to serialize protobuf message (Topic: %s): %s", message->topic, PB_GET_ERROR(&stream));
        return U_ERROR;
    }

    /* Publish over MQTT. */
    status = ethernet_mqtt_publish(message->topic, message->topic_size, (char*)buffer, stream.bytes_written); // u_TODO - ethernet_mqtt_publish should return U_SUCCESS/U_ERROR instead of the internal netx error macro
    if(status != NXD_MQTT_SUCCESS) {
        PRINTLN_WARNING("Failed to publish MQTT message (Topic: %s, Status: %d).", message->topic, status);

        /* If disconnected, attempt reconnection. */
        if(status == NXD_MQTT_NOT_CONNECTED) {
            PRINTLN_WARNING("Detected disconnect from MQTT. Attempting reconnection...");
            do {
                tx_thread_sleep(1000);
                status = ethernet_mqtt_reconnect();
                PRINTLN_WARNING("Attempting MQTT reconnection (Status: %d).", status);
            } while ((status != NXD_MQTT_SUCCESS) && (status != NXD_MQTT_ALREADY_CONNECTED));
            PRINTLN_WARNING("MQTT reconnection successful.");
        }

        return U_ERROR;
    }

    /* Return successful! */
    PRINTLN_INFO("Sent MQTT message (Topic: %s).", message->topic);
    return U_SUCCESS;
}
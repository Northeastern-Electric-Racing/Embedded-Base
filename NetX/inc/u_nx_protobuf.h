#pragma once

// clang-format off

/*
*   Wrapper for structuring and sending protobuf Ethernet messages over MQTT. 
*   This API sits above the lower-level u_nx_ethernet.h driver layer. Messages with nonstandard formats can still be sent using the u_nx_ethernet.h API directly.
*/

#include <stddef.h>
#include <stdbool.h>
#include "serverdata.pb.h"

/* Helper macros. */
#define PB_STR_HELPER(x) #x                                                   // Helper for PB_TOSTR(). Probably should never use directly.
#define PB_TOSTR(x) PB_STR_HELPER(x)                                          // Converts a macro's value into a string.
#define PB_COUNT_ARGS(...) (sizeof((float[]){ __VA_ARGS__ }) / sizeof(float)) // Returns the number of arguments passed into it.
#define PB_STR_LEN(s) (sizeof(s) - 1)                                         // Returns the length of a string literal.

/* CONFIG: Compile-time validation of topic size, unit size, and number of values. */
#define PB_MAX_TOPIC_LENGTH 100 // Maximum
#define PB_MAX_UNIT_LENGTH  15
#define PB_MIN_DATAPOINTS   1
#define PB_MAX_DATAPOINTS   5
#define PB_VALIDATE_ARGS(topic, unit, num_values)                                                                                             \
    do {                                                                                                                                      \
        _Static_assert(                                                                                                                       \
            PB_STR_LEN(topic) <= PB_MAX_TOPIC_LENGTH,                                                                                         \
            "MQTT topic parameter exceeds maximum length of " PB_TOSTR(PB_MAX_TOPIC_LENGTH) " allowed by `nx_protobuf_mqtt_message_create()`."\
        );                                                                                                                                    \
        _Static_assert(                                                                                                                       \
            PB_STR_LEN(unit) <= PB_MAX_UNIT_LENGTH,                                                                                           \
            "MQTT unit parameter exceeds maximum length of " PB_TOSTR(PB_MAX_UNIT_LENGTH) " allowed by `nx_protobuf_mqtt_message_create()`."  \
        );                                                                                                                                    \
        _Static_assert(                                                                                                                       \
            (num_values) >= PB_MIN_DATAPOINTS,                                                                                                \
            "Must pass at least " PB_TOSTR(PB_MIN_DATAPOINTS) " value into the variable argument of `nx_protobuf_mqtt_message_create()`."     \
        );                                                                                                                                    \
        _Static_assert(                                                                                                                       \
            (num_values) <= PB_MAX_DATAPOINTS,                                                                                                \
            "Cannot pass more than " PB_TOSTR(PB_MAX_DATAPOINTS) " values into the variable argument of `nx_protobuf_mqtt_message_create()`." \
        );                                                                                                                                    \
    } while (0)

/**
 * @brief Creates and formats a `ethernet_mqtt_message_t` object, and returns it to the caller.
 * @param topic (const char*) String literal representing the message's MQTT topic name.
 * @param unit (const char*) String literal representing the unit of the message's data.
 * @param ... (float) The data to be sent in the message. This is a variable argument, so it can be repeated depending on how many datapoints you want to send. If you pass in more datapoints than allowed, you will get a compile-time error.
 * @return An `ethernet_mqtt_message_t` object. 
 * @note If message creation was not completed for any reason, .initialized will be false in the returned `ethernet_mqtt_message_t` object. You may still use the object as you please (including attempting to initialize it again), but attempting to send the message (via `nx_protobuf_mqtt_message_send()`) will return an error.
 */
#define nx_protobuf_mqtt_message_create(topic, unit, ...)           \
    ({                                                              \
        PB_VALIDATE_ARGS(topic, unit, PB_COUNT_ARGS(__VA_ARGS__));  \
        _nx_protobuf_mqtt_message_create(                           \
            (topic), PB_STR_LEN(topic),                             \
            (unit),  PB_STR_LEN(unit),                              \
            (float[]){ __VA_ARGS__ },                               \
            PB_COUNT_ARGS(__VA_ARGS__)                              \
        );                                                          \
    })


/* Ethernet MQTT Message. */
typedef struct {
    const char* topic;
    int topic_size;
    serverdata_v2_ServerData protobuf;
    bool initialized;
} ethernet_mqtt_message_t;

/**
 * @brief Dispatches a `ethernet_mqtt_message_t` message over MQTT.
 * @param message The message to send.
 * @return U_SUCCESS if successful, U_ERROR is not successful.
 */
int nx_protobuf_mqtt_message_send(ethernet_mqtt_message_t* message);

/* MACRO IMPLEMENTATIONS */
ethernet_mqtt_message_t _nx_protobuf_mqtt_message_create(const char* topic, size_t topic_size, const char* unit,  size_t unit_len, const float values[], int values_count);

// clang-format on
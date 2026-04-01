#pragma once

// clang-format off

/*
*   Wrapper for structuring and sending protobuf Ethernet messages over MQTT. 
*   This API sits above the lower-level u_nx_ethernet.h driver layer. Messages with nonstandard formats can still be sent using the u_nx_ethernet.h API directly.
*/

#include <stddef.h>
#include <stdbool.h>
#include "serverdata.pb.h"

/* CONFIG: Compile-time validation of topic size, unit size, and number of values. */
#define PB_VALIDATE_ARGS(topic, unit, num_values)                                                               \
    do {                                                                                                        \
        _Static_assert(                                                                                         \
            sizeof(topic) <= 100,                                                                               \
            "MQTT topic parameter exceeds maximum length of 100 allowed by `nx_protobuf_mqtt_message_create()`."\
        );                                                                                                      \
        _Static_assert(                                                                                         \
            sizeof(unit) <= 15,                                                                                 \
            "MQTT unit parameter exceeds maximum length of 15 allowed by `nx_protobuf_mqtt_message_create()`."  \
        );                                                                                                      \
        _Static_assert(                                                                                         \
            (num_values) >= 1,                                                                                  \
            "Must pass at least 1 value into the variable argument of `nx_protobuf_mqtt_message_create()`."     \
        );                                                                                                      \
        _Static_assert(                                                                                         \
            (num_values) <= 5,                                                                                  \
            "Cannot pass more than 5 values into the variable argument of `nx_protobuf_mqtt_message_create()`." \
        );                                                                                                      \
    } while (0)


/* Helper macros. */
#define PB_COUNT_ARGS(...) (sizeof((float[]){ __VA_ARGS__ }) / sizeof(float)) // Returns the number of arguments passed into it.
#define PB_STR_LEN(s) (sizeof(s) - 1)                                         // Returns the length of a string literal.

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
    serverdata_v2_ServerData msg;
    bool initialized;
} ethernet_mqtt_message_t;

/* MACRO IMPLEMENTATIONS */
ethernet_mqtt_message_t _nx_protobuf_mqtt_message_create(const char* topic, size_t topic_len, const char* unit,  size_t unit_len, const float values[], int values_size);

// clang-format on
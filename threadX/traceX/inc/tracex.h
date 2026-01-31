/**
 * @file tracex.h
 * @brief TraceX control interface.
 *
 */

#ifndef TRACEX_H
#define TRACEX_H

#include "tx_api.h"
#include <stdint.h>

/**
 * @brief Initialize TraceX with a user-provided buffer.
 *
 * This must be called once before starting TraceX.
 *
 * @param buffer Pointer to trace buffer.
 * @param size   Size of trace buffer in bytes.
 */
void tracex_init(UCHAR *buffer, uint32_t size);

/**
 * @brief Start TraceX recording.
 *
 * Safe to call multiple times.
 */
void tracex_start(void);

/**
 * @brief Stop TraceX recording.
 *
 * Safe to call multiple times.
 */
void tracex_stop(void);

/**
 * @brief Get the TraceX buffer pointer.
 *
 * @return Pointer to trace buffer.
 */
UCHAR *tracex_get_buffer(void);

/**
 * @brief Get the TraceX buffer size.
 *
 * @return Buffer size in bytes.
 */
uint32_t tracex_get_buffer_size(void);

#endif /* TRACEX_H */
/**
 * @file traceout.h
 * @brief TraceX data output interface.
 */

#ifndef TRACEOUT_H
#define TRACEOUT_H

#include <stdint.h>

/* Size of each transmitted chunk */
#define TRACEOUT_CHUNK_BYTES (1024U)

/**
 * @brief Platform transmit callback.
 *
 * Must initiate a non-blocking transmit (DMA / IT).
 *
 * @param data Pointer to data buffer.
 * @param len  Number of bytes to transmit.
 */
typedef void (*traceout_tx_fn_t)(const uint8_t *data, uint16_t len);

/**
 * @brief Initialize trace output streaming.
 *
 * @param tx_fn Platform transmit callback.
 */
void traceout_init(traceout_tx_fn_t tx_fn);

/**
 * @brief Start trace output from ISR context.
 */
void traceout_start_from_isr(void);

/**
 * @brief Notify trace output that transmit completed (ISR context).
 */
void traceout_on_tx_complete_from_isr(void);

#endif /* TRACEOUT_H */
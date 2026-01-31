/**
 * @file traceout.c
 * @brief TraceX data output interface.
 */

#include "traceout.h"
#include "tracex.h"

static traceout_tx_fn_t s_tx_fn = NULL;
static volatile uint8_t s_output_active = 0U;
static const uint8_t *s_dp = NULL;
static uint32_t s_remaining = 0U;

static void start_next_chunk(void) {
  if (s_remaining == 0U) {
    s_output_active = 0U;
    tracex_start();
    return;
  }

  uint32_t n =
      (s_remaining > TRACEOUT_CHUNK_BYTES) ? TRACEOUT_CHUNK_BYTES : s_remaining;

  s_tx_fn(s_dp, (uint16_t)n);
  s_dp += n;
  s_remaining -= n;
}

void traceout_init(traceout_tx_fn_t tx_fn) { s_tx_fn = tx_fn; }

void traceout_start_from_isr(void) {
  if ((s_tx_fn == NULL) || (s_output_active != 0U)) {
    return;
  }

  tracex_stop();

  s_dp = tracex_get_buffer();
  s_remaining = tracex_get_buffer_size();
  s_output_active = 1U;

  start_next_chunk();
}

void traceout_on_tx_complete_from_isr(void) {
  if (s_output_active != 0U) {
    start_next_chunk();
  }
}
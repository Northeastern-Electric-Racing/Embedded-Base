/**
 * @file tracex.c
 * @brief TraceX control interface.
 *
 */

#include "tracex.h"
#include <stdbool.h>

/**
 * @brief Enable the CPU cycle counter used by TraceX timestamps.
 *
 * This function is implemented by the application or platform layer.
 * It must enable a free-running CPU cycle counter before TraceX is started.
 *
 * It is called once from tracex_start().
 */
void tracex_enable_cycle_counter(void);

static UCHAR *s_trace_buffer = NULL;
static uint32_t s_trace_size = 0U;
static bool s_tracex_started = false;

void tracex_init(UCHAR *buffer, uint32_t size)
{
	s_trace_buffer = buffer;
	s_trace_size = size;
}

UCHAR *tracex_get_buffer(void)
{
	return s_trace_buffer;
}

uint32_t tracex_get_buffer_size(void)
{
	return s_trace_size;
}

void tracex_start(void)
{
	if ((s_tracex_started == true) || (s_trace_buffer == NULL) ||
	    (s_trace_size == 0U)) {
		return;
	}

	tracex_enable_cycle_counter();
	tx_trace_enable(s_trace_buffer, s_trace_size, 32U);
	s_tracex_started = true;
}

void tracex_stop(void)
{
	if (s_tracex_started == false) {
		return;
	}

	tx_trace_disable();
	s_tracex_started = false;
}
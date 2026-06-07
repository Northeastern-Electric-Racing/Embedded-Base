#ifndef CDEFMT_CONFIG_H
#define CDEFMT_CONFIG_H

/* ---------------------------------------------------------------------------
 * Stack-allocated log buffer (default).
 *
 * Each CDEFMT_* call materializes a packed struct on the caller's stack of
 * size: sizeof(log_id) + sum(sizeof(args)) + DYNAMIC_SIZE_MAX bytes. Threads
 * that log must have enough stack headroom to cover this.
 * ------------------------------------------------------------------------- */
#define CDEFMT_USE_STACK_LOG_BUFFER              1
#define CDEFMT_STACK_LOG_BUFFER_DYNAMIC_SIZE_MAX 64

/* ---------------------------------------------------------------------------
 * Alternative buffer modes (disabled by default).
 *
 * If switching to STATIC, also provide CDEFMT_STATIC_LOG_BUFFER,
 * CDEFMT_STATIC_LOG_BUFFER_SIZE, and a lock/unlock pair if multi-threaded.
 *
 * If switching to DYNAMIC, also provide CDEFMT_DYNAMIC_LOG_BUFFER_ALLOC and
 * CDEFMT_DYNAMIC_LOG_BUFFER_FREE.
 * ------------------------------------------------------------------------- */
#define CDEFMT_USE_STATIC_LOG_BUFFER  0
#define CDEFMT_USE_DYNAMIC_LOG_BUFFER 0

#endif /* CDEFMT_CONFIG_H */

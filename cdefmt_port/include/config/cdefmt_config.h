/*
 * Default cdefmt configuration for projects that consume Embedded-Base's
 * `embedded_base_cdefmt` CMake target.
 *
 * cdefmt's header (`cdefmt/include/cdefmt.h`) does:
 *     #include <config/cdefmt_config.h>
 * so this file must live at `<some-include-dir>/config/cdefmt_config.h`.
 * The `embedded_base_cdefmt` target adds the parent directory of `config/`
 * to the include path, so this file is picked up automatically.
 *
 * To override these defaults from a parent project, ensure that an earlier
 * include directory provides a different `config/cdefmt_config.h`.
 */

#ifndef CDEFMT_CONFIG_H
#define CDEFMT_CONFIG_H

/* ---------------------------------------------------------------------------
 * Stack-allocated log buffer (default).
 *
 * Each CDEFMT_* call materializes a packed struct on the caller's stack of
 * size: sizeof(log_id) + sum(sizeof(args)) + DYNAMIC_SIZE_MAX bytes. Threads
 * that log must have enough stack headroom to cover this. For a typical
 * Cortex-M project with multi-kB thread stacks, the default 128-byte
 * dynamic reservation is comfortable.
 * ------------------------------------------------------------------------- */
#define CDEFMT_USE_STACK_LOG_BUFFER              1
#define CDEFMT_STACK_LOG_BUFFER_DYNAMIC_SIZE_MAX 128

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


#ifndef __U_THREADX_H
#define __U_THREADX_H

#include "tx_api.h"
#include "u_threads.h"
#include <stdint.h>

uint8_t _create_thread(TX_BYTE_POOL *byte_pool, thread_t *thread);

#endif
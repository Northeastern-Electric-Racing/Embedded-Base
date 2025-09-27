#ifndef __U_TX_GENERAL_H
#define __U_TX_GENERAL_H

#include "tx_port.h"

/* Time and tick conversions */
#define MS_TO_TICKS(ms) (((ms) * TX_TIMER_TICKS_PER_SECOND + 999) / 1000)
#define TICKS_TO_MS(ticks)  ((ticks) * 1000 / TX_TIMER_TICKS_PER_SECOND)

#endif
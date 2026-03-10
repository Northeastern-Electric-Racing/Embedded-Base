#ifndef TSECU_SHEPHERD_RTC_H
#define TSECU_SHEPHERD_RTC_H

#include "nxd_ptp_client.h"
#include "nx_api.h"

UINT nx_ptp_client_hard_clock_callback(NX_PTP_CLIENT *client_ptr,
				       UINT operation, NX_PTP_TIME *time_ptr,
				       NX_PACKET *packet_ptr,
				       VOID *callback_data);

#endif //TSECU_SHEPHERD_RTC_H
#include "nxd_ptp_client.h"
#include "u_rtc.h"
#include "stm32h5xx_hal.h"
#include "u_tx_debug.h"
#include "tx_api.h"
#include <time.h>

/// This file implements the PTP client callback to manage time.
/// This is where the interface between the PTP client and the timebase is.
/// This was written using RM0481 and UM3131.
///
/// The timebase for the clock used is listed as clk_ptp_ref_i.
/// The PTP Offload feature is NOT utilized because it is very half-baked.
/// Instead we simply use the internal clock reference and a userspace UDP impl.
///
/// TUNING: The status of this operation can be found by using eth_ptp_pps_out.
///
/// Docs: This mostly follows the procedure in page RM0481 2792 57.9.9
/// This uses coarse timestamping binary rollover mode
///

extern ETH_HandleTypeDef heth;

#define PTP_UTC_OFFSET 0 // UTC 0


UINT nx_ptp_client_hard_clock_callback(NX_PTP_CLIENT *client_ptr,
				       UINT operation, NX_PTP_TIME *time_ptr,
				       NX_PACKET *packet_ptr,
				       VOID *callback_data)
{
	TX_INTERRUPT_SAVE_AREA
	switch (operation) {
		case NX_PTP_CLIENT_CLOCK_INIT:
			TX_DISABLE

			// see 57.9.9

			// Mask the Timestamp Trigger interrupt
			CLEAR_BIT(heth.Instance->MACIER, ETH_MACIER_TSIE);
			SET_BIT(heth.Instance->MACTSCR, ETH_MACTSCR_TSENA);

			SET_BIT(heth.Instance->MACTSCR, ETH_MACTSCR_TSCTRLSSR); // 1 ns

			// each step is 1ns, with a clock of clk_ptp_ref_i
			// clk_ptp_ref_i comes from pll1_q_clk (eth clock) (pg 456)
			// this cool API gives us the frequency (in hz)
			PLL1_ClocksTypeDef pll1;
			HAL_RCCEx_GetPLL1ClockFreq(&pll1);
			uint8_t period_ns = 1000000000 / pll1.PLL1_Q_Frequency;
			MODIFY_REG(heth.Instance->MACSSIR, ETH_MACMACSSIR_SNSINC, period_ns << ETH_MACMACSSIR_SNSINC_Pos);

			TX_RESTORE
			break;

		case NX_PTP_CLIENT_CLOCK_SET:
			/// A large jump in time
			TX_DISABLE

			// use overwrite, not add
			WRITE_REG(heth.Instance->MACSTNUR, time_ptr->nanosecond);

			// WARNING travelers of future: this code suffers from the year 2106 problem
			// Use seconds_high to account for this, but note the silicon also has this issue
			WRITE_REG(heth.Instance->MACSTSUR, time_ptr->second_low);


			SET_BIT(heth.Instance->MACTSCR, ETH_MACTSCR_TSINIT);

			while (READ_BIT(heth.Instance->MACTSCR, ETH_MACTSCR_TSINIT)) {}
			// clear the seconds register now so we dont need to do it for a subsecond update
			CLEAR_REG(heth.Instance->MACSTSUR);

			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_PACKET_TS_EXTRACT: // FALL THROUGH
		case NX_PTP_CLIENT_CLOCK_GET:
			/// Retrieve time, usually for packet timestamping
			TX_DISABLE

			time_ptr->nanosecond = READ_REG(heth.Instance->MACSTNR);
            time_ptr->second_low = READ_REG(heth.Instance->MACSTSR);
			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_ADJUST:
			/// A small adjustment in time (<1 second)
			TX_DISABLE
			// use add, not overwrite
			if (time_ptr->nanosecond >= 0) {
				// this will automatically clear the ADDSUB so we add
				WRITE_REG(heth.Instance->MACSTNUR, time_ptr->nanosecond);
			} else {
				// take complement starting from one million
				uint32_t complement = 0x3B9AC9FF - time_ptr->second_low;
				WRITE_REG(heth.Instance->MACSTSUR, complement);
				SET_BIT(heth.Instance->MACSTNUR, ETH_MACSTNUR_ADDSUB);
			}


			SET_BIT(heth.Instance->MACTSCR, ETH_MACTSCR_TSUPDT);

			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_PACKET_TS_PREPARE:
			/// Push a packet with a timestamp

			time_ptr->nanosecond = READ_REG(heth.Instance->MACSTNR);
			time_ptr->second_low = READ_REG(heth.Instance->MACSTSR);

			nx_ptp_client_packet_timestamp_notify(client_ptr, packet_ptr, time_ptr);
			break;
		case NX_PTP_CLIENT_CLOCK_SOFT_TIMER_UPDATE: // do nothing
			break;
		default:
			PRINTLN_ERROR("How Did We Get Here? (rtc)");
			break;
	}

	return NX_SUCCESS;
}

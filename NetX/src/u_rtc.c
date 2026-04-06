#include "nxd_ptp_client.h"
#include "rtc.h"
#include <stdio.h>
#include "stm32h5xx_hal.h"
#include "u_tx_debug.h"
#include <time.h>

#define PTP_UTC_OFFSET 0 // UTC 0
extern RTC_HandleTypeDef hrtc;

UINT interrupt_save; // do not remove

static UINT us_to_second_ticks(ULONG us, UINT second_fractions)
{
	// Second fraction = SS / (PREDIV_S + 1)
	return (uint64_t)us * (second_fractions + 1) / 1000000L;
}

static ULONG second_ticks_to_us(UINT second_ticks, UINT second_fractions)
{
	return (uint64_t)1000000L * second_ticks / (second_fractions + 1);
}

static void set_subsecond(UINT rtc_sub_second_tick, UINT second_fractions, NX_PTP_DATE_TIME *current_time)
{
	if (rtc_sub_second_tick > second_fractions) {
		PRINTLN_ERROR("rtc SS overflow");
	}

	UINT rtp_sub_second_tick = us_to_second_ticks(
		current_time->nanosecond / 1000, second_fractions);

	UINT offset_tick = 0; // ticks to go backwards
	UINT offset_ahead_1s = RTC_SHIFTADD1S_RESET;
	if (rtc_sub_second_tick >= rtp_sub_second_tick) { // local ahead
		offset_tick = rtc_sub_second_tick - rtp_sub_second_tick;
	} else { // local behind
		offset_ahead_1s = RTC_SHIFTADD1S_SET;
		UINT delta = rtp_sub_second_tick - rtc_sub_second_tick;
		offset_tick = (second_fractions + 1) - delta;
	}
	HAL_RTCEx_SetSynchroShift(&hrtc, offset_tick, offset_ahead_1s);
}

UINT nx_ptp_client_hard_clock_callback(NX_PTP_CLIENT *client_ptr,
				       UINT operation, NX_PTP_TIME *time_ptr,
				       NX_PACKET *packet_ptr,
				       VOID *callback_data)
{
	NX_PTP_DATE_TIME *current_date_time;
	RTC_TimeTypeDef rtc_time = {0};
	RTC_DateTypeDef rtc_date = {0};

	switch (operation) {
		case NX_PTP_CLIENT_CLOCK_INIT:
			break;

		case NX_PTP_CLIENT_CLOCK_SET:
			TX_DISABLE

			nx_ptp_client_utility_convert_time_to_date(
				time_ptr, -PTP_UTC_OFFSET, current_date_time);

			rtc_time = (RTC_TimeTypeDef) {
				.Hours = current_date_time->hour,
				.Minutes = current_date_time->minute,
				.Seconds = current_date_time->second,
				.TimeFormat = 0,
			};

			rtc_date = (RTC_DateTypeDef) {
				.Year = current_date_time->year % 100,
				.Month = current_date_time->month,
				.Date = current_date_time->day,
				.WeekDay = current_date_time->weekday,
			};

			HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

			// After SetTime, SubSeconds is reset to SecondFraction (0 elapsed).
			// Shift from 0 elapsed ticks to wherever PTP nanosecond says we should be.
			RTC_TimeTypeDef after_set = {};
			HAL_RTC_GetTime(&hrtc, &after_set, RTC_FORMAT_BIN); // to get a valid SecondFraction
			set_subsecond(0, after_set.SecondFraction, current_date_time);
			
			// dummy get date
			HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_PACKET_TS_EXTRACT: // FALL THROUGH
		case NX_PTP_CLIENT_CLOCK_GET:
			TX_DISABLE

			HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

			NX_PTP_DATE_TIME rtc_ptp_date_time = {
				.year = rtc_date.Year,
				.month = rtc_date.Month,
				.weekday = rtc_date.WeekDay,
				.day = rtc_date.Date,
				.hour = rtc_time.Hours,
				.minute = rtc_time.Minutes,
				.second = rtc_time.Seconds,
				.nanosecond =
					1000 * second_ticks_to_us(
						rtc_time.SecondFraction - rtc_time.SubSeconds,						       
						rtc_time.SecondFraction)
			};

			// helpful way to get UNIX time from RTC so we can give it to NetX layer
			time_t current_time_unix = { 0 };
			struct tm tim = {0};
			tim.tm_year = rtc_date.Year + 100;
			tim.tm_mon = rtc_date.Month - 1;
			tim.tm_mday = rtc_date.Date;
			tim.tm_hour = rtc_time.Hours;
			tim.tm_min = rtc_time.Minutes;
			tim.tm_sec = rtc_time.Seconds;
			current_time_unix = mktime(&tim);


			time_ptr->second_high = 0; // todo fix by 2038
			time_ptr->second_low = (ULONG) current_time_unix;
			time_ptr->nanosecond =
				rtc_ptp_date_time
					.nanosecond; // pull directly from rtc

			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_ADJUST:
			TX_DISABLE

			nx_ptp_client_utility_convert_time_to_date(
				time_ptr, -PTP_UTC_OFFSET, current_date_time);

			HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);

			set_subsecond(
				rtc_time.SecondFraction - rtc_time.SubSeconds,
				rtc_time.SecondFraction, current_date_time); // (between 0 and PREDIV_S (aka SecondFraction), counting up)

			TX_RESTORE

			// dummy get date
			HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
			break;
		case NX_PTP_CLIENT_CLOCK_PACKET_TS_PREPARE:
			nx_ptp_client_packet_timestamp_notify(
				client_ptr, packet_ptr, time_ptr); // this is probably wrong, instead we need to fetch the time via RTC
			break;
		case NX_PTP_CLIENT_CLOCK_SOFT_TIMER_UPDATE: // do nothing
			break;
		default:
			PRINTLN_ERROR("How Did We Get Here? (rtc.h)");
			break;
	}

	return NX_SUCCESS;
}

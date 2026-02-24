#include "rtc.h"
#include <stdio.h>
#include "stm32h5xx_hal_rtc.h"

#define PTP_UTC_OFFSET 0 // UTC 0
extern RTC_HandleTypeDef hrtc;

NX_PTP_TIME *ptp_time;
NX_PTP_DATE_TIME *ptp_date_time;

UINT interrupt_save;

static UINT us_to_second_ticks(ULONG us, UINT second_fractions)
{
	// Second fraction = SS / (PREDIV_S + 1)
	return (uint64_t)us * (second_fractions + 1) / 1000000L;
}

static ULONG second_ticks_to_us(UINT second_ticks, UINT second_fractions)
{
	return (uint64_t)1000000L * second_ticks / (second_fractions + 1);
}

static void set_subsecond(UINT rtc_sub_second_tick, UINT second_fractions)
{
	if (~(rtc_sub_second_tick >> 15 & 1)) {
		printf("rtc SS overflow ig");
	}

	UINT rtp_sub_second_tick = us_to_second_ticks(
		ptp_date_time->nanosecond / 1000, second_fractions);

	UINT offset_tick = 0; // ticks to go backwards
	UINT offset_ahead_1s = RTC_SHIFTADD1S_RESET;
	if (rtc_sub_second_tick >= rtp_sub_second_tick) { // local ahead
		offset_tick = rtc_sub_second_tick - rtp_sub_second_tick;
	} else { // local behind
		offset_ahead_1s = RTC_SHIFTADD1S_SET;
		offset_tick = second_fractions -
			      (rtc_sub_second_tick - rtp_sub_second_tick);
	}
	HAL_RTCEx_SetSynchroShift(&hrtc, offset_tick, offset_ahead_1s);
}

static LONG diff_ptp_date_time(NX_PTP_DATE_TIME *time1, NX_PTP_DATE_TIME *time2)
{
#define SECS_PER_MINUTE 60
#define SECS_PER_HOUR	(60 * SECS_PER_MINUTE)
#define SECS_PER_DAY	(24 * SECS_PER_HOUR)
#define SECS_PER_YEAR	(365 * SECS_PER_DAY)

	int64_t seconds_diff = 0;

	seconds_diff += (time2->year - time1->year) * SECS_PER_YEAR;
	seconds_diff += (time2->day - time1->day) * SECS_PER_DAY;
	seconds_diff += (time2->hour - time1->hour) * SECS_PER_HOUR;
	seconds_diff += (time2->minute - time1->minute) * SECS_PER_MINUTE;
	seconds_diff += (time2->second - time1->second) * 1;

	return seconds_diff;
}

UINT nx_ptp_client_hard_clock_callback(NX_PTP_CLIENT *client_ptr,
				       UINT operation, NX_PTP_TIME *time_ptr,
				       NX_PACKET *packet_ptr,
				       VOID *callback_data)
{
	switch (operation) {
		case NX_PTP_CLIENT_CLOCK_INIT:
			break;

		case NX_PTP_CLIENT_CLOCK_SET:
			TX_DISABLE

			ptp_time = time_ptr;

			nx_ptp_client_utility_convert_time_to_date(
				ptp_time, -PTP_UTC_OFFSET, ptp_date_time);

			RTC_TimeTypeDef rtp_time = {
				.Hours = ptp_date_time->hour,
				.Minutes = ptp_date_time->minute,
				.Seconds = ptp_date_time->second,
				.TimeFormat = 0,
			};

			RTC_DateTypeDef rtp_date = {
				.Year = ptp_date_time->year,
				.Month = ptp_date_time->month,
				.Date = ptp_date_time->day,
				.WeekDay = ptp_date_time->weekday,
			};

			// NOTE: 24 hours RTC assumed.
			RTC_TimeTypeDef rtc_sub_seconds = {};

			HAL_RTC_GetTime(&hrtc, &rtc_sub_seconds,
					RTC_FORMAT_BCD);

			HAL_RTC_SetTime(&hrtc, &rtp_time, RTC_FORMAT_BCD);

			HAL_RTC_SetDate(&hrtc, &rtp_date, RTC_FORMAT_BCD);

			set_subsecond(rtc_sub_seconds.SecondFraction -
					      rtc_sub_seconds.SubSeconds,
				      rtc_sub_seconds.SubSeconds);
			TX_RESTORE
		case NX_PTP_CLIENT_CLOCK_PACKET_TS_EXTRACT: // FALL THROUGH
		case NX_PTP_CLIENT_CLOCK_GET:
			TX_DISABLE
			RTC_TimeTypeDef rtc_time = {};
			RTC_DateTypeDef rtc_date = {};

			HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BCD);
			HAL_RTCEx_GetTimeStamp(&hrtc, &rtc_time, &rtc_date,
					       RTC_FORMAT_BCD);

			NX_PTP_DATE_TIME rtc_ptp_date_time = {
				.year = rtp_date.Year,
				.month = rtp_date.Month,
				.weekday = rtp_date.WeekDay,
				.day = rtp_date.Date,
				.hour = rtc_time.Hours,
				.minute = rtc_time.Minutes,
				.second = rtc_time.Seconds,
				.nanosecond =
					1000 * second_ticks_to_us(
						       rtc_time.SubSeconds,
						       rtc_time.SecondFraction)
			};

			int64_t secondsDiff = diff_ptp_date_time(
				ptp_date_time, &rtc_ptp_date_time);

			NX_PTP_TIME current_ptp_time = {
				.second_high = ptp_time->second_high,
				.second_low = ptp_time->second_low,
				.nanosecond = 0
			};

			if (secondsDiff >
			    0) { // ahead of previous ptp time stamp
				_nx_ptp_client_utility_add64(
					&current_ptp_time.second_high,
					&current_ptp_time.second_low, 0,
					secondsDiff);
			} else {
				_nx_ptp_client_utility_sub64(
					&current_ptp_time.second_high,
					&current_ptp_time.second_low, 0,
					secondsDiff);
			}

			time_ptr->second_high = current_ptp_time.second_high;
			time_ptr->second_low = current_ptp_time.second_low;
			time_ptr->nanosecond =
				rtc_ptp_date_time
					.nanosecond; // pull directly from rtc

			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_ADJUST:
			TX_DISABLE
			ptp_time = time_ptr;

			nx_ptp_client_utility_convert_time_to_date(
				ptp_time, -PTP_UTC_OFFSET, ptp_date_time);

			HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BCD);

			set_subsecond(
				rtc_time.SecondFraction - rtc_time.SubSeconds,
				rtc_time.SubSeconds); // (between 0 and PREDIV_S (aka SecondFraction), counting up)

			TX_RESTORE
			break;
		case NX_PTP_CLIENT_CLOCK_PACKET_TS_PREPARE:
			nx_ptp_client_packet_timestamp_notify(
				client_ptr, packet_ptr, ptp_time);
			break;
		case NX_PTP_CLIENT_CLOCK_SOFT_TIMER_UPDATE: // do nothing
			break;
		default:
			printf("How Did We Get Here? (rtc.h)");
			break;
	}

	return NX_SUCCESS;
}
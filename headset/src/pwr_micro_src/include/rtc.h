/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * rtc.h - Real time clock initialization and configuration
 */

#ifndef RTC_H_
#define RTC_H_

#include <stm32l1xx.h>

// Value for Alarm A register to specify period of 1s
#define RTC_1S (RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1)
// Value for Alarm A register to specify period of 60s (every time seconds == 0)
#define RTC_1M (RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2)

/**
 * Initializes the real-time clock and low-speed oscillator, and prepares the microcontroller
 * for the necessary low-power modes.
 */
void rtcInit(void);
/**
 * Sets the RTC alarm interrupt to RTC_1S or RTC_1M.
 *
 * @param value the RTC alarm wake up frequency to set
 */
void rtcSetAlarmFrequency(const uint32_t freq);

#endif

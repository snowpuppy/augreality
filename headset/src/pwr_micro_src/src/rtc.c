/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * rtc.c - Real time clock initialization and configuration
 */

#include "main.h"

/**
 * Initializes the real-time clock and low-speed oscillator, and prepares the microcontroller
 * for the necessary low-power modes.
 */
void rtcInit(void) {
	EXTI_InitTypeDef exti;
	// Unlock for write access
	PWR->CR |= PWR_CR_DBP;
	__DSB();
	// RTC clocks on, pick the LSI as the source
	RCC->CSR = (RCC->CSR & ~RCC_CSR_RTCSEL) | RCC_CSR_RTCEN | RCC_CSR_RTCSEL_LSI;
	// Enter initialization mode
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	__DSB();
	RTC->ISR |= RTC_ISR_INIT;
	while (!(RTC->ISR & RTC_ISR_INITF));
	// Configure RTC prescalars for 1 Hz clock (38 KHz nom / (124 + 1) / (303 + 1) ~= 1 Hz)?
	RTC->PRER = (RTC->PRER & ~RTC_PRER_PREDIV_A) | (uint32_t)(127U << 16);
	RTC->PRER = (RTC->PRER & ~RTC_PRER_PREDIV_S) | (uint32_t)(256U);
	// Set up RTC Alarm A to trip every second
	RTC->ALRMAR = RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1;
	RTC->CR |= RTC_CR_ALRAIE | RTC_CR_ALRAE;
	// Reset clock to zero
	RTC->TR = 0x0U;
	RTC->DR = 0x0U;
	// Exit initialization mode and enable write protection
	RTC->ISR &= ~RTC_ISR_INIT;
	RTC->WPR = 0xFFU;
	// Configure EXTI line 17 to interrupt in rising edge mode
	exti.EXTI_Line = EXTI_Line17;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti);
	// Clear any pending interrupts
	RTC->ISR &= ~RTC_ISR_ALRAF;
	EXTI->PR &= EXTI_PR_PR17;
	// Enable the RTC alarm in NVIC
	NVIC_SetPriority(RTC_Alarm_IRQn, 3);
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	// Remove me for lower power usage once working
	//DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP;
	// Make the default low-power mode Sleep, not Stop
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP;
}

/**
 * Sets the RTC alarm interrupt to RTC_1S or RTC_1M.
 *
 * @param value the RTC alarm wake up frequency to set
 */
void rtcSetAlarmFrequency(const uint32_t freq) {
	// Disable write protection
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	__DSB();
	// Disable the alarm register and wait for availability
	RTC->ISR &= ~RTC_ISR_ALRAF;
	RTC->CR &= ~RTC_CR_ALRAE;
	while (!(RTC->ISR & RTC_ISR_ALRAWF));
	// Set up RTC Alarm A to trip at desired frequency
	RTC->ALRMAR = freq;
	RTC->CR |= RTC_CR_ALRAE;
	__DSB();
	// Enable write protection
	RTC->WPR = 0xFFU;
}

/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * clocks.c - Clock initialization and calibration functions
 */

#include "main.h"

/**
 * Switches the system clock to the HSE+PLL at the maximum value of 32 MHz. Leaves the MSI
 * running once complete. The FLASH is configured to 1 wait state and the voltage regulator is
 * set to VR1 (1.8 V).
 *
 * This mode enables the USB clock. Other peripherals may need to be reconfigured.
 *
 * If the HSE fails to start up, the clocks will remain set to their current state.
 */
void switchToHSE(void) {
	uint32_t startUpCounter = HSE_STARTUP_TIMEOUT;
	// Turn the HSE on
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY) && --startUpCounter > 0);
	if (startUpCounter > 0) {
		// Switch FLASH interface to 1 WS
		FLASH->ACR |= FLASH_ACR_LATENCY;
#ifdef REDUCE_VOLTAGE
		// Select Voltage Range 1 (1.8 V) required for USB
		PWR->CR = (PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS_0;
		__DSB();
		// Wait until the voltage regulator is ready
		while (PWR->CSR & PWR_CSR_VOSF);
#endif
		// PLL configuration: *12, /3 (overall X4)
		RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV)) |
			RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL12 | RCC_CFGR_PLLDIV3;
		// Enable PLL
		RCC->CR |= RCC_CR_PLLON;
		// Wait till PLL is ready
		while (!(RCC->CR & RCC_CR_PLLRDY) && --startUpCounter > 0);
		if (startUpCounter > 0) {
			// Select PLL as system clock source
			RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
			// Wait till PLL is used as system clock source
			while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL && --startUpCounter > 0);
		}
	}
	// If we fail, shut off the PLL+HSE
	if (startUpCounter == 0)
		RCC->CR &= ~(RCC_CR_PLLON | RCC_CR_HSEON);
}

/**
 * Switches the system clock to the MSI. Disables the HSE and PLL once complete. The MSI will
 * retain the frequency it last had. The FLASH is configured to 0 wait states and the voltage
 * regulator is set to VR3 (1.2 V).
 *
 * This mode disables the USB clock, but USB wakeup is still possible. Other peripherals may
 * need to be reconfigured.
 */
void switchToMSI(void) {
	// The MSI will be selected as system clock (this allows us to come up, but w/o USB)
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_MSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);
#ifdef REDUCE_VOLTAGE
	// Reduce operating voltage to VR3
	PWR->CR |= PWR_CR_VOS_0 | PWR_CR_VOS_1;
	__DSB();
	// Wait until the voltage regulator is ready
	while (PWR->CSR & PWR_CSR_VOSF);
#endif
	// Shut off the power hogs
	RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_PLLON);
	// Switch FLASH interface to 0 WS
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
}

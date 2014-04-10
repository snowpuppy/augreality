/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * main.c - Power supply microcontroller source code
 */

#include "main.h"

/**
 * USB initialization
 *
 * Initializes the USB peripheral and turns on the internal USB pull-up resistor
 */
static void initUSB(void) {
	GPIO_InitTypeDef gpio;
	// Power up USB
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
	__DSB();
	// Set up the pins for USB
	// NOTE The pin mode MUST be input! NOT alternate function!
	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &gpio);
	// Set AF modes for USB
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USB);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USB);
	// Turn on USB pull-up resistor
	SYSCFG->PMC |= SYSCFG_PMC_USB_PU;
	usbInit();
	// Enable USB IRQ
	NVIC_SetPriority(USB_LP_IRQn, 2);
	NVIC_EnableIRQ(USB_LP_IRQn);
}

/**
 * Low-power mode, RTC, and LSI
 *
 * Initializes the real-time clock and low-speed oscillator, and prepares the microcontroller
 * for the necessary low-power modes.
 */
static void initLP(void) {
	EXTI_InitTypeDef exti;
	// EXTI clocks on
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
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
	RTC_ExitInitMode();
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
	// XXX Remove me for lower power usage when working
	DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP;
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP;
}

/**
 * System initialization
 *
 * Initializes the interrupt system, enables GPIO clocks, and sets up each peripheral
 */
static void init(void) {
	// Priority group #3 configuration
	NVIC_SetPriorityGrouping(3);
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
	// Set up the peripherals
	i2cInit();
	serialInit();
	initLP();
	initUSB();
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
}

static bool send = false;

/**
 * Called by the startup to run the main program
 */
int main(void) {
	// Sys init
	init();
	while (1) {
		// Loop bytes back
		if (usbIsConnected() && send) {
			uint8_t value[2];
			// Get voltage registers from the fuel gauge
			if (i2cReadRegister(0x34, 0x0C, value, 2)) {
				// DS2782 Figure 4: V = 9876543210XXXXX
				uint32_t volts = ((uint32_t)value[0] << 3) | ((uint32_t)value[1] >> 5);
				// 1 lsb = 4.88 mV, 39/8 = 4.875 mV, calculate voltage
				printf("%u\r\n", (unsigned int)((volts * 39) >> 3));
			}
			send = false;
		}
		SLEEP();
	}
	return 0;
}

/**
 * Called every 1s to read the fuel gauge
 */
void IRQ RTC_Alarm_IRQHandler(void) {
	// Reset the alarm clock
	RTC->ISR &= ~RTC_ISR_ALRAF;
	EXTI->PR &= EXTI_PR_PR17;
	send = true;
}

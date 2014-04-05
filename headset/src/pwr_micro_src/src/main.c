/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * Power supply microcontroller source code
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
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
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
	initUSB();
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
	// XXX Remove me for lower power usage when working
	DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP;
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP;
}

/**
 * Called by the startup to run the main program
 */
int main(void) {
	// Sys init
	init();
	while (1) {
		// Loop bytes back
		if (usbIsConnected() && usbAcmCount() > 0) {
			uint16_t value;
			usbAcmPut(usbAcmGet());
			// TEST: Get voltage registers (wrong endian order!) from the fuel gauge
			if (i2cReadRegister(0x34, 0x0C, (uint8_t *)&value, 2))
				printf("%04X\r\n", value);
		}
		SLEEP();
	}
	return 0;
}

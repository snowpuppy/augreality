/* Includes */

#include "main.h"
#include "printf.h"
#include "usb_cdcacm.h"

/**
 * System initialization
 */
static void init(void) {
	GPIO_InitTypeDef gpio;
	// Set up the peripherals
	//i2cInit();
	serialInit();
	// Power up USB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
	// Set up the pins for USB
	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &gpio);
	// Set AF modes for USB
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USB);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USB);
	// Turn on USB pull-up resistor
	usbInit();
	SYSCFG->PMC |= SYSCFG_PMC_USB_PU;
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
}

int main(void) {
	// Sys init
	init();
	while (1) {
		// Loop bytes back
		if (usbIsConnected()) {
			while (usbAcmCount() > 0)
				// Send them back
				usbAcmPut(usbAcmGet());
		}
		__WFI();
	}
	return 0;
}

/* Includes */

#include "main.h"
#include "printf.h"
#include "usb.h"

/**
 * System initialization
 */
static void init(void) {
	// Set up the peripherals
	//i2cInit();
	serialInit();
	usbVCPInit();
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
}

int main(void) {
	uint8_t data[64];
	// Sys init
	init();
	while (1) {
		// Loop bytes back
		uint32_t count = serialBufferCount(SERIAL_PORT_USB);
		if (count > 0) {
			// Pull up to 64 bytes
			if (count > 64) count = 64;
			usbVCPRead(data, count);
			// Send them back
			usbVCPWrite(data, count);
		}
		__WFI();
	}
	return 0;
}

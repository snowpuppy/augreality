/* Includes */

#include "main.h"
#include "imu.h"
#include "printf.h"
#include "usb.h"

/**
 * USB initialization
 */
static void usbInit(void) {
	VCP_Init();
	VCP_SetUSBTxBlocking(0);
}

/**
 * System initialization
 */
static void init(void) {
	// Tick every 1ms
	SysTick_Config(168000);
	// Set up status LED
	gpioInit();
	// Set up the peripherals
	i2cInit();
	serialInit();
	usbInit();
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
}

void spiReceivedByte(uint8_t data) {
}

int main(void) {
	uint8_t data[64];
	// Sys init
	init();
	ledOff();
	while (1) {
		// Loop bytes back
		uint32_t count = VCP_BytesAvailable();
		if (count > 0) {
			// Pull up to 64 bytes
			if (count > 64) count = 64;
			for (uint32_t i = 0; i < count; i++)
				data[i] = VCP_GetByte();
			// Send them back
			VCP_DataTx(data, count);
		}
		__WFI();
	}
	return 0;
}

/* Includes */

#include "main.h"
#include "itm.h"
#include "printf.h"
#include <limits.h>

unsigned volatile long sysTime = 0UL;

// The string that will be returned...
static const char returnString[] = "Hello World";
#define RETURN_STRING_LEN ((sizeof(returnString) - 1) / sizeof(char))
static uint32_t returnIndex = 0;

void msleep(unsigned long howLong) {
	unsigned long then = sysTime + howLong;
	while (sysTime < then) __WFI();
}

// User callback invoked whenever a byte is RXed over SPI
extern void spiReceivedByte(uint8_t data) {
	uint32_t gi = returnIndex;
	ledToggle(LED6);
	// Write the byte
	spiWriteByte((uint8_t)returnString[gi++]);
	// Wrap around if lots of bytes received
	if (gi >= RETURN_STRING_LEN) gi = 0;
	returnIndex = gi;
}

void init(void) {
	// Tick every 1ms
	buttonInit(BUTTON_MODE_GPIO);
	ledInit();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// Enable interrupts for systick and faults
	SysTick_Config(168000);
	__enable_fault_irq();
	__enable_irq();
}

int main(void) {
	// Sys init
	returnIndex = 2;
	// Stuff the first two characters (one fills the shift register, one fills the buffer)
	spiWriteByte(returnString[0]);
	spiWriteByte(returnString[1]);
	init();
	spiInit();
	itmInit();
	ledOn(LED6);
	// Wait FOREVER!
	while (1) __WFI();
	return 0;
}

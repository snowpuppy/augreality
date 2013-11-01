/* Includes */

#include "main.h"
#include "itm.h"
#include "printf.h"

unsigned volatile long sysTime = 0UL;

void msleep(unsigned long howLong) {
	unsigned long then = sysTime + howLong;
	while (sysTime < then) __WFI();
}

void init(void) {
	// Tick every 1ms
	SysTick_Config(168000);
	// Set up status LED
	ledInit();
	// Enable interrupts for systick and faults
	__enable_fault_irq();
	__enable_irq();
}

int main(void) {
	uint8_t data[4];
	unsigned int soc, v;
	GPIO_InitTypeDef gs;
	// Sys init
	init();
	i2cInit();
	itmInit();
	ledOff(LED1);
	while (1) {
		if (i2cReadRegister(0x36, 0x02, data, 4)) {
			v = ((unsigned int)data[0] << 4) | ((unsigned int)data[1] >> 4);
			// 2.5 mV/LSB
			v = (v * 5) >> 2;
			soc = (unsigned int)data[2];
			// Report SOC
			printf("Volt:%dmV SOC:%d%%\n", (int)v, (int)soc);
			ledOn(LED1);
		} else
			ledOff(LED1);
		msleep(1000UL);
	}
	return 0;
}

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
	buttonInit(BUTTON_MODE_GPIO);
	ledInit();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// Enable interrupts for systick and faults
	SysTick_Config(168000);
	__enable_fault_irq();
	__enable_irq();
}

int main(void) {
	uint8_t data[4];
	unsigned int soc, v;
	// Sys init
	init();
	i2cInit();
	itmInit();
	ledOff(LED6);
	while (1) {
		ledOn(LED6);
		if (i2cReadRegister(0x36, 0x02, data, 4)) {
			v = ((unsigned int)data[0] << 4) | ((unsigned int)data[1] >> 4);
			// 2.5 mV/LSB
			v = (v * 5) >> 2;
			soc = (unsigned int)data[2];
			// Report SOC
			printf("Volt:%dmV SOC:%d%%\n", (int)v, (int)soc);
			ledOn(LED3);
		} else
			// Error reading
			ledOff(LED3);
		ledOff(LED6);
		msleep(1000UL);
	}
	return 0;
}

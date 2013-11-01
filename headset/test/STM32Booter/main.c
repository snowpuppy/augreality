/* Includes */
#include "main.h"
#include "stm32f4xx_gpio.h"

static volatile unsigned long long millis = 0LL;

int main(void) {
	GPIO_InitTypeDef gpio;
	SysTick_Config(168000);
	__enable_irq();
	// Tick every 1ms
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// Set up heart beat LED
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &gpio);
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		msleep(500L);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		msleep(500L);
	}
	return 0;
}

void _tick() {
	millis++;
}

void msleep(long time) {
	unsigned long long target = millis + time;
	while (millis < target) __WFI();
}

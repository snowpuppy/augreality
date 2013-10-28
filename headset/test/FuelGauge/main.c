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
	GPIO_InitTypeDef gp;
	USART_InitTypeDef us;
	USART_ClockInitTypeDef uc;
	// Tick every 1ms
	buttonInit(BUTTON_MODE_GPIO);
	ledInit();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// USART 3 up
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	gp.GPIO_OType = GPIO_OType_PP;
	gp.GPIO_Speed = GPIO_PuPd_NOPULL;
	gp.GPIO_Mode = GPIO_Mode_AF;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &gp);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	// USART 3 config
	us.USART_BaudRate = 115200;
	us.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	us.USART_Parity = USART_Parity_No;
	us.USART_StopBits = USART_StopBits_1;
	us.USART_WordLength = USART_WordLength_8b;
	// Half duplex TX only
	us.USART_Mode = USART_Mode_Tx;
	USART_Init(USART3, &us);
	// Clocking mode
	uc.USART_Clock = USART_Clock_Disable;
	uc.USART_LastBit = USART_LastBit_Disable;
	uc.USART_CPHA = USART_CPHA_1Edge;
	uc.USART_CPOL = USART_CPOL_High;
	USART_ClockInit(USART3, &uc);
	USART_Cmd(USART3, ENABLE);
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

/*
 * gpio.c - GPIO code for the STM32F4xx specific to the actual PCB implementation
 */

#include "gpio.h"
#include "main.h"

/**
 * Initialize the LEDs and other GPIO on the actual PCB.
 */
void gpioInit() {
	GPIO_InitTypeDef GPIO_InitStructure;
	// Set up clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// Set up LED pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * Turn on the LED on the PCB.
 */
void ledOn() {
	GPIOC->BSRRL = GPIO_Pin_13;
}

/**
 * Turn off the LED on the PCB.
 */
void ledOff() {
	GPIOC->BSRRH = GPIO_Pin_13;
}

/**
 * Toggle the LED on the PCB.
 */
void ledToggle() {
	GPIOC->ODR ^= GPIO_Pin_13;
}

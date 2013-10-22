/**
  ******************************************************************************
  * @file    stm32f4_discovery.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on STM32F4-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

#include "stm32f4_discovery.h"

static const uint16_t masks[] = { GPIO_Pin_13, GPIO_Pin_12, GPIO_Pin_14, GPIO_Pin_15 };

/**
 * Initialize the LEDs on the STM32F4Discovery board.
 * NOTE! This function needs to be changed when initializing Debug LEDs on the actual PCB
 */
void ledInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/**
 * Turn on the specified LED as marked on the STM32F4Discovery PCB, from LED3 to LED6
 *
 * @param led the LED to turn on, either LED3, LED4, LED5, or LED6
 */
void ledOn(LED_TypeDef led) {
	GPIOD->BSRRL = masks[led];
}

/**
 * Turn off the specified LED as marked on the STM32F4Discovery PCB, from LED3 to LED6
 *
 * @param led the LED to turn off, either LED3, LED4, LED5, or LED6
 */
void ledOff(LED_TypeDef led) {
	GPIOD->BSRRH = masks[led];
}

/**
 * Toggle the specified LED as marked on the STM32F4Discovery PCB, from LED3 to LED6
 *
 * @param led the LED to toggle, either LED3, LED4, LED5, or LED6
 */
void ledToggle(LED_TypeDef led) {
	GPIOD->ODR ^= masks[led];
}

/**
 * Initialize the user button on the STM32F4Discovery board.
 *
 * @param mode the button mode, either BUTTON_MODE_EXTI or BUTTON_MODE_GPIO
 */
void buttonInit(ButtonMode_TypeDef mode) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the BUTTON Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure Button pin as input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	if (mode == BUTTON_MODE_EXTI) {
		/* Connect Button EXTI Line to Button GPIO Pin */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

		/* Configure Button EXTI line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set Button EXTI Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);
	}
}

/**
 * Gets the value of the user button if configured as GPIO.
 *
 * @return the button value, 1 for pressed and 0 for released
 */
uint32_t buttonGet() {
	return GPIOA->IDR & GPIO_Pin_0;
}

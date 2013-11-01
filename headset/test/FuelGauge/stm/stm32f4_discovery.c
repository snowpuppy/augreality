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

static const uint16_t masks[] = { GPIO_Pin_13 };

/**
 * Initialize the LEDs on the actual PCB
 */
void ledInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * Turn on the specified LED as marked on the STM32F4Discovery PCB, from LED3 to LED6
 *
 * @param led the LED to turn on, either LED3, LED4, LED5, or LED6
 */
void ledOn(LED_TypeDef led) {
	GPIOC->BSRRL = masks[led];
}

/**
 * Turn off the specified LED as marked on the STM32F4Discovery PCB, from LED3 to LED6
 *
 * @param led the LED to turn off, either LED3, LED4, LED5, or LED6
 */
void ledOff(LED_TypeDef led) {
	GPIOC->BSRRH = masks[led];
}

/**
 * Toggle the specified LED as marked on the STM32F4Discovery PCB, from LED3 to LED6
 *
 * @param led the LED to toggle, either LED3, LED4, LED5, or LED6
 */
void ledToggle(LED_TypeDef led) {
	GPIOC->ODR ^= masks[led];
}

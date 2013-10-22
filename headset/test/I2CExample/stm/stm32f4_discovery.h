/**
 ******************************************************************************
 * @file    stm32f4_discovery.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    19-September-2011
 * @brief   This file contains definitions for STM32F4-Discovery Kit's Leds and
 *          push-button hardware resources.
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

#ifndef STM32F4_DISCOVERY_H_
#define STM32F4_DISCOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

typedef enum {
	LED4 = 0, LED3 = 1, LED5 = 2, LED6 = 3
} LED_TypeDef;

typedef enum {
	BUTTON_MODE_GPIO = 0, BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

void ledInit();
void ledOn(LED_TypeDef led);
void ledOff(LED_TypeDef led);
void ledToggle(LED_TypeDef led);
uint32_t buttonGet();
void buttonInit(ButtonMode_TypeDef Button_Mode);

#ifdef __cplusplus
}
#endif

#endif

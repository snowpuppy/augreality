/*
 * gpio.c - GPIO code for the STM32F4xx specific to the actual PCB implementation
 */

#include "gpio.h"
#include "main.h"

// RSSI pin change timeout in us
#define RSSI_TIMEOUT 75U
#define RSSI_FLAG_FALLING 0x01

// RSSI data structure
static struct {
	volatile uint16_t rssi;
	volatile uint16_t flags;
	volatile uint32_t lastValue;
} rssiData;

/**
 * Gets the detected PWM input on the RSSI pin, corresponding to the received signal strength
 * indicator for the XBee wireless unit.
 *
 * @return the measured PWM edge width in microseconds from 1 to 200, or 0 if no valid edges
 * are captured
 */
uint32_t gpioGetRSSI() {
	return (uint32_t)rssiData.rssi;
}

/**
 * Initialize the LEDs and other GPIO on the actual PCB.
 */
void gpioInit() {
	GPIO_InitTypeDef gpio;
	TIM_ICInitTypeDef ic;
	TIM_OCInitTypeDef oc;
	TIM_TimeBaseInitTypeDef tim;
	// Reset RSSI structure
	rssiData.rssi = 0U;
	rssiData.lastValue = 0U;
	rssiData.flags = 0x00;
	// Set up clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// Power on TIM5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	// Set up LED pin
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio);
	// Set up the RSSI pin as alternate function input pull-up
	gpio.GPIO_Pin = GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &gpio);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);
	// Set up TIM1 to tick at 1us, auto reload at maximum (200 us expected max PWM width)
	// APB1 = SYSCLK/4 = 168/4 = 42 MHz
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 83U;
	tim.TIM_Period = 0xFFFFFFFFU;
	tim.TIM_RepetitionCounter = 0U;
	TIM_TimeBaseInit(TIM5, &tim);
	// Set up TIM5 for input capture on channel 2
	ic.TIM_Channel = TIM_Channel_2;
	ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	ic.TIM_ICFilter = 0U;
	TIM_ICInit(TIM5, &ic);
	// Set up TIM5 compare interrupt OC1 for the "timed out" indicator
	oc.TIM_OCMode = TIM_OCMode_Timing;
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	oc.TIM_OCPolarity = TIM_OCPolarity_High;
	oc.TIM_OCNPolarity = TIM_OCNPolarity_High;
	oc.TIM_OutputState = TIM_OutputState_Disable;
	oc.TIM_OutputNState = TIM_OutputState_Disable;
	TIM_OC1Init(TIM5, &oc);
	// Turn on the capture/compare lines
	TIM_SetCounter(TIM5, 0U);
	TIM5->CCR1 = 200U;
	TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
	// Enable the TIM5 interrupts
	TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);
	// Enable TIM5
	TIM_Cmd(TIM5, ENABLE);
	NVIC_SetPriority(TIM5_IRQn, 4);
	NVIC_EnableIRQ(TIM5_IRQn);
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

// TIM5 input capture
void __attribute__ (( interrupt("IRQ") )) TIM5_IRQHandler() {
	// This can occur on CCR1 (timed out) or CC2 (signal received)
	uint16_t sr = TIM5->SR;
	uint16_t rssi = rssiData.rssi;
	if (sr & TIM_SR_CC1IF) {
		// Indicate maxed-out low or maxed-out high signal strength on PA1 respectively
		if (GPIOA->IDR & 0x0002)
			rssi = RSSI_TIMEOUT;
		else
			rssi = 0U;
		// Re-arm the timeout counter
		TIM5->CCR1 = TIM5->CNT + RSSI_TIMEOUT;
		// Shut off the alarm clock
		TIM5->SR = ~TIM_SR_CC1IF;
	}
	if (sr & TIM_SR_CC2IF) {
		uint32_t tim = TIM5->CCR2;
		uint16_t flags = rssiData.flags;
		if (flags & RSSI_FLAG_FALLING) {
			// Falling edge seen, calculate absolute difference (if lastValue > tim, then the
			// 32-bit wrap around will resolve correctly, e.g. 0x00000001 - 0xFFFFFFFF = 2)
			uint32_t dt = tim - rssiData.lastValue;
			flags = flags & ~RSSI_FLAG_FALLING;
			TIM5->CCER &= ~TIM_CCER_CC2P;
			// If pulse was too long, it is invalid, return 0
			if (dt < RSSI_TIMEOUT)
				rssi = (uint16_t)dt;
			else
				rssi = 0U;
		} else {
			// Rising edge seen
			rssiData.lastValue = tim;
			TIM5->CCER |= TIM_CCER_CC2P;
			flags = flags | RSSI_FLAG_FALLING;
		}
		// Arm the timed-out counter
		TIM5->CCR1 = TIM5->CNT + RSSI_TIMEOUT;
		// Store out the flags again
		rssiData.flags = flags;
		// Shut off the alarm clock
		TIM5->SR = ~TIM_SR_CC2IF;
	}
	rssiData.rssi = rssi;
}

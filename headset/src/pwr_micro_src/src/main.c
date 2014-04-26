/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * main.c - Power supply microcontroller source code
 */

#include "main.h"

// Flag bits for the flags byte
#define FLAG_REPORT 0x01U
#define FLAG_ALARM 0x02U
#define FLAG_STOP 0x04U

// Flags byte used to handle reporting over USB
static volatile uint16_t flags;

/**
 * GPIO initialization
 *
 * Turn on the GPIO, set the unused pins to input pull down, then kill clocks on unused
 * modules
 */
static void initGPIO(void) {
	GPIO_InitTypeDef gpio;
	// Turn all GPIO on
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN |
		RCC_AHBENR_GPIODEN;
	__DSB();
	// PA11, PA12 are USB; PA13, PA14, PA15 are JTAG (PA9 and PA10 are the serial port)
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
	gpio.GPIO_Speed = GPIO_Speed_400KHz;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_Pin = (uint16_t)0x07FFU;
	GPIO_Init(GPIOA, &gpio);
	// PB10, PB11 are I2C; PB2 has external pull down
	gpio.GPIO_Pin = (uint16_t)0xF3FBU;
	GPIO_Init(GPIOB, &gpio);
	// All PC unused
	gpio.GPIO_Pin = (uint16_t)0xFFFFU;
	GPIO_Init(GPIOC, &gpio);
	// PD2 only pin
	gpio.GPIO_Pin = (uint16_t)0xFFFFU;
	GPIO_Init(GPIOD, &gpio);
	// Turn off C and D which are not used
	RCC->AHBENR &= ~(RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN);
}

/**
 * USB initialization
 *
 * Initializes the USB peripheral and turns on the internal USB pull-up resistor
 */
static void initUSB(void) {
	EXTI_InitTypeDef exti;
	GPIO_InitTypeDef gpio;
	// Power up USB
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
	__DSB();
	// Set up the pins for USB
	// NOTE The pin mode MUST be input! NOT alternate function!
	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &gpio);
	// Set AF modes for USB
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USB);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USB);
	// Turn on USB pull-up resistor
	SYSCFG->PMC |= SYSCFG_PMC_USB_PU;
	usbInit();
	// Configure EXTI line 18 to interrupt in rising edge mode
	exti.EXTI_Line = EXTI_Line18;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti);
	// Clear any pending interrupts
	EXTI->PR &= EXTI_PR_PR18;
	// Enable USB IRQs
	NVIC_SetPriority(USB_LP_IRQn, 2);
	NVIC_EnableIRQ(USB_LP_IRQn);
	NVIC_SetPriority(USB_FS_WKUP_IRQn, 3);
	NVIC_EnableIRQ(USB_FS_WKUP_IRQn);
}

/**
 * System initialization
 *
 * Initializes the interrupt system, enables GPIO clocks, and sets up each peripheral
 */
static void init(void) {
	flags = 0x00U;
	// Priority group #3 configuration
	NVIC_SetPriorityGrouping(3);
	initGPIO();
	// EXTI and SYSCFG clocks on
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// Set up the peripherals, the debug serial is not used anymore so shut it off for power
	i2cInit();
	rtcInit();
	initUSB();
	// Enable interrupts for all peripherals
	__enable_fault_irq();
	__enable_irq();
}

/**
 * Prepares for STOP low-power mode, arming RTC for 1 minute. Switches the clock to MSI.
 *
 * This should probably be called from USB suspend.
 */
static void enterStopMode(void) {
	switchToMSI();
	flags |= FLAG_STOP;
	rtcSetAlarmFrequency(RTC_1M);
}

/**
 * Prepares for normal run mode, arming RTC for 1 second. Switches the clock to HSE+PLL.
 *
 * This should probably be called from USB resume.
 */
static void exitStopMode(void) {
	switchToHSE();
	flags &= ~FLAG_STOP;
	rtcSetAlarmFrequency(RTC_1S);
}

/**
 * Called by the startup to run the main program
 */
int main(void) {
	// Avoid excess reads of global variable
	uint32_t lFlags;
	// Temporary for I2C transactions
	uint8_t value[2];
	// Initialize the system
	init();
	while (1) {
		// Disable interrupts to safely read and write flags variable
		__disable_irq();
		lFlags = flags;
		if ((lFlags & FLAG_ALARM) != 0)
			// Shut off the alarm clock
			flags = lFlags & ~FLAG_ALARM;
		__enable_irq();
		// Get voltage registers from the fuel gauge every second
		if (lFlags & FLAG_ALARM) {
			if (usbIsConnected() && i2cReadRegister(0x34, 0x0C, value, 2)) {
				// DS2782 datasheet Figure 4: V = 9876543210XXXXX
				uint32_t volts = ((uint32_t)value[0] << 3) | ((uint32_t)value[1] >> 5);
				// 1 lsb = 4.88 mV, 39/8 = 4.875 mV, calculate voltage
				printf("%u\r\n", (unsigned int)((volts * 39) >> 3));
			}
		} else {
			// Go into STOP mode if unplugged, or SLEEP if plugged ni
			if (lFlags & FLAG_STOP)
				STOP();
			else
				SLEEP();
		}
	}
	return 0;
}

/**
 * Called when the USB interface receives a suspend event, indicating that the host has powered
 * down.
 */
void usbPowerDown(void) {
	// Disable reporting over USB
	flags &= ~(FLAG_ALARM | FLAG_REPORT);
	// Enter STOP mode
	enterStopMode();
}

/**
 * Called every 1s to read the fuel gauge.
 */
void IRQ RTC_Alarm_IRQHandler(void) {
	// Shut off the alarm clock
	RTC->ISR &= ~RTC_ISR_ALRAF;
	EXTI->PR &= EXTI_PR_PR17;
	// Read flags
	const uint32_t lFlags = flags;
	if (lFlags & FLAG_REPORT)
		// Assert ALARM signal if needed
		flags = lFlags | FLAG_ALARM;
}

/**
 * Called on USB wake-up detected.
 */
void IRQ USB_FS_WKUP_IRQHandler(void) {
	// Shut off the alarm clock
	EXTI->PR &= EXTI_PR_PR18;
	// Exit STOP mode, as the host has connected and powered up
	exitStopMode();
	// Reset the USB peripheral to prepare for re-enumeration
	SYSCFG->PMC |= SYSCFG_PMC_USB_PU;
	usbInit();
	// Enable reporting over USB
	flags = (flags & ~FLAG_ALARM) | FLAG_REPORT;
}

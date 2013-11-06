/*
 * gpio.h - GPIO code for the STM32F4xx specific to the actual PCB implementation
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

/**
 * Gets the detected PWM input on the RSSI pin, corresponding to the received signal strength
 * indicator for the XBee wireless unit.
 *
 * @return the measured PWM edge width in microseconds from 1 to 200, or 0 if no valid edges
 * are captured
 */
uint32_t gpioGetRSSI();
/**
 * Initialize the LEDs and other GPIO on the actual PCB.
 */
void gpioInit();
/**
 * Turn on the LED on the PCB.
 */
void ledOn();
/**
 * Turn off the LED on the PCB.
 */
void ledOff();
/**
 * Toggle the LED on the PCB.
 */
void ledToggle();

#endif

/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * gpio.h - GPIO code for the STM32F4xx specific to the actual PCB implementation
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

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

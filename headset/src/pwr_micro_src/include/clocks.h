/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * clocks.h - Clock initialization and calibration functions
 */

#ifndef CLOCKS_H_
#define CLOCKS_H_

#include <stm32l1xx.h>

/**
 * Switches the system clock to the HSE+PLL at the maximum value of 32 MHz. Leaves the MSI
 * running once complete. The FLASH is configured to 1 wait state and the voltage regulator is
 * set to VR1 (1.8 V).
 *
 * This mode enables the USB clock. Other peripherals may need to be reconfigured.
 *
 * If the HSE fails to start up, the clocks will remain set to their current state.
 */
void switchToHSE(void);
/**
 * Switches the system clock to the MSI. Disables the HSE and PLL once complete. The MSI will
 * retain the frequency it last had. The FLASH is configured to 0 wait states and the voltage
 * regulator is set to VR3 (1.2 V).
 *
 * This mode disables the USB clock, but USB wakeup is still possible. Other peripherals may
 * need to be reconfigured.
 */
void switchToMSI(void);

#endif

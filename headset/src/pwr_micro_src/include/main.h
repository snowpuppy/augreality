/*
 * main.h - Main include file for microcontroller code
 */

#ifndef MAIN_H_
#define MAIN_H_

/*
 * Required C includes
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Pull in required include files for all peripherals
 */
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"

/**
 * msleep - Delay in low-power mode for the given number of milliseconds.
 *
 * @param howLong the delay period in milliseconds
 */
void msleep(unsigned long howLong);

#endif

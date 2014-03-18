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
#include "arm_math.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_it.h"
#include "gpio.h"
#include "i2c.h"
#include "comm.h"
#include "main.h"

/**
 * msleep - Delay in low-power mode for the given number of milliseconds.
 *
 * @param howLong the delay period in milliseconds
 */
void msleep(unsigned long howLong);

#endif

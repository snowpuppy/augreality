/*
 * main.h - Main include file for microcontroller code
 */

#ifndef MAIN_H_
#define MAIN_H_

// Required C includes
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Pull in required include files for all peripherals
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "comm.h"
#include "i2c.h"
#include "usb_cdcacm.h"
#include "printf.h"

// Goes into Sleep mode, peripherals continue to run
#define SLEEP() do { __WFI(); asm volatile ("nop\n\t"); } while (0)
// Goes into STOP mode, peripherals are stopped but SRAM is retained
#define STOP() do { SCB->SCR |= SCB_SCR_SLEEPDEEP; __WFI(); asm volatile ("nop\n\t"); SCB->SCR &= ~SCB_SCR_SLEEPDEEP; } while (0)

#endif

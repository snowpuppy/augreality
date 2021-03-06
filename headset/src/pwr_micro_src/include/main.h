/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
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
#include "clocks.h"
#include "comm.h"
#include "i2c.h"
#include "usb_cdcacm.h"
#include "rtc.h"
#include "printf.h"

// Defines an IRQ function
#define IRQ __attribute__ ((interrupt("IRQ")))
// Goes into Sleep mode, peripherals continue to run
#define SLEEP() do { __WFI(); asm volatile ("nop\n\t"); } while (0)
// Goes into STOP mode, peripherals are stopped but SRAM is retained
#define STOP() do { PWR->CR |= PWR_CR_LPSDSR; \
	PWR->CR |= PWR_CR_CWUF; \
	SCB->SCR |= SCB_SCR_SLEEPDEEP; \
	__DSB(); \
	__WFI(); \
	asm volatile ("nop\n\t"); \
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP; \
} while (0)

#endif

/*
 * API_ext.h - extended API functions useful for advanced Maple programmers who seek to
 * utilize cutting-edge functionality or write a driver for a custom sensor
 *
 * This file is part of the Purdue Robotics Operating System (PROS).
 *
 * PROS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PROS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef API_EXT_H_
#define API_EXT_H_

#include "API.h"

// Begin C++ extern to C
#ifdef __cplusplus
extern "C" {
#endif

// ---- Advanced input and output definitions ----

// Pin/port configuration for ioSetDirection
// Analog input
#define DDR_INPUT_ANALOG 0x00
// Floating input
#define DDR_INPUT_FLOATING 0x04
// Input with weak pull down
#define DDR_INPUT_PULLDOWN 0x08
// Input with weak pull up
#define DDR_INPUT_PULLUP 0x0A
// General-purpose output
#define DDR_OUTPUT 0x01
// Open-drain output
#define DDR_OUTPUT_OD 0x05
// Alternate function output
#define DDR_AFO 0x09
// Alternate function open-drain output
#define DDR_AFO_OD 0x0D

// 42 available digital I/O
#define BOARD_NR_GPIO 42
// 15 available analog I/O (pin 13's ADC is not all that useful...)
#define BOARD_NR_ANALOG 15

// Peripheral memory map: addresses in the direct region
// Flash base address
#define FLASH_BASE ((uint32_t)0x08000000)
// RAM base address
#define SRAM_BASE ((uint32_t)0x20000000)
// Peripheral base address
#define PERIPH_BASE ((uint32_t)0x40000000)
// SCS base address
#define SCS_BASE ((uint32_t)0xE000E000)
// APB1 peripherals base address
#define APB1PERIPH_BASE PERIPH_BASE
// APB2 peripherals base address
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000)
// AHB peripherals base address
#define AHBPERIPH_BASE (PERIPH_BASE + 0x20000)
// GPIO port A
#define GPIOA_BASE (APB2PERIPH_BASE + 0x0800)
#define GPIOA ((GPIOPort*)GPIOA_BASE)
// GPIO port B
#define GPIOB_BASE (APB2PERIPH_BASE + 0x0C00)
#define GPIOB ((GPIOPort*)GPIOB_BASE)
// GPIO port C
#define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)
#define GPIOC ((GPIOPort*)GPIOC_BASE)
// GPIO port D
#define GPIOD_BASE (APB2PERIPH_BASE + 0x1400)
#define GPIOD ((GPIOPort*)GPIOD_BASE)

// Pinning definitions for ioSetDirection
// Digital pins 0-14 (D5-D9 and D14 are 5V tolerant)
#define PIN_DIGITAL_0 GPIOA, 3
#define PIN_DIGITAL_1 GPIOA, 2
#define PIN_DIGITAL_2 GPIOA, 0
#define PIN_DIGITAL_3 GPIOA, 1
#define PIN_DIGITAL_4 GPIOB, 5
#define PIN_DIGITAL_5 GPIOB, 6
#define PIN_DIGITAL_6 GPIOA, 8
#define PIN_DIGITAL_7 GPIOA, 9
#define PIN_DIGITAL_8 GPIOA, 10
#define PIN_DIGITAL_9 GPIOB, 7
#define PIN_DIGITAL_10 GPIOA, 4
#define PIN_DIGITAL_11 GPIOA, 7
#define PIN_DIGITAL_12 GPIOA, 6
#define PIN_DIGITAL_13 GPIOA, 5
#define PIN_LED PIN_DIGITAL_13
#define PIN_DIGITAL_14 GPIOB, 8
// Digital pins 15-20 (analog 0-5)
#define PIN_DIGITAL_15 GPIOC, 0
#define PIN_DIGITAL_16 GPIOC, 1
#define PIN_DIGITAL_17 GPIOC, 2
#define PIN_DIGITAL_18 GPIOC, 3
#define PIN_DIGITAL_19 GPIOC, 4
#define PIN_DIGITAL_20 GPIOC, 5
// Digital pins 21-37 on the expansion connector (D24-26 and D29-42 are 5V tolerant)
#define PIN_DIGITAL_21 GPIOC, 13
#define PIN_DIGITAL_22 GPIOC, 14
#define PIN_DIGITAL_23 GPIOC, 15
#define PIN_DIGITAL_24 GPIOB, 9
#define PIN_DIGITAL_25 GPIOD, 2
#define PIN_DIGITAL_26 GPIOC, 10
#define PIN_DIGITAL_27 GPIOB, 0
#define PIN_DIGITAL_28 GPIOB, 1
#define PIN_DIGITAL_29 GPIOB, 10
#define PIN_DIGITAL_30 GPIOB, 11
#define PIN_DIGITAL_31 GPIOB, 12
#define PIN_DIGITAL_32 GPIOB, 13
#define PIN_DIGITAL_33 GPIOB, 14
#define PIN_DIGITAL_34 GPIOB, 15
#define PIN_DIGITAL_35 GPIOC, 6
#define PIN_DIGITAL_36 GPIOC, 7
#define PIN_DIGITAL_37 GPIOC, 8
// Digital pin 38 is the button pin
#define PIN_DIGITAL_38 GPIOC, 9
#define PIN_BUTTON PIN_DIGITAL_38
// Digital pins 39-42 on the JTAG connector (43 is a reset)
#define PIN_DIGITAL_39 GPIOA, 13
#define PIN_DIGITAL_40 GPIOA, 14
#define PIN_DIGITAL_41 GPIOA, 15
#define PIN_DIGITAL_42 GPIOB, 3
// Analog inputs 1-6 (the other ADCs are on digital 0-3, digital 10-13, and digital 27-28)
// None of these pins are 5V tolerant
#define PIN_ANALOG_0 PIN_DIGITAL_15
#define PIN_ANALOG_1 PIN_DIGITAL_16
#define PIN_ANALOG_2 PIN_DIGITAL_17
#define PIN_ANALOG_3 PIN_DIGITAL_18
#define PIN_ANALOG_4 PIN_DIGITAL_19
#define PIN_ANALOG_5 PIN_DIGITAL_20
// Communications ports (not all 5V tolerant, see Digital pins)
#define PIN_UART1_TX PIN_DIGITAL_7
#define PIN_UART1_RX PIN_DIGITAL_8
#define PIN_UART2_TX PIN_DIGITAL_1
#define PIN_UART2_RX PIN_DIGITAL_0
#define PIN_UART3_TX PIN_DIGITAL_29
#define PIN_UART3_RX PIN_DIGITAL_30
#define PIN_I2C1_SCL PIN_DIGITAL_5
#define PIN_I2C1_SDA PIN_DIGITAL_9
#define PIN_I2C2_SCL PIN_DIGITAL_29
#define PIN_I2C2_SDA PIN_DIGITAL_30
#define PIN_SPI1_SCK PIN_DIGITAL_13
#define PIN_SPI1_MISO PIN_DIGITAL_12
#define PIN_SPI1_MOSI PIN_DIGITAL_11
#define PIN_SPI1_NSS PIN_DIGITAL_10
#define PIN_SPI2_SCK PIN_DIGITAL_32
#define PIN_SPI2_MISO PIN_DIGITAL_33
#define PIN_SPI2_MOSI PIN_DIGITAL_34
#define PIN_SPI2_NSS PIN_DIGITAL_31

// Opaque typedef for GPIO pinning
typedef char GPIOPort;

// ---- Advanced input and output routines ----

// adcOff - Stops the ADC, use before reconfiguring
void adcOff();
// adcOn - Starts the ADC
void adcOn();
// adcRead - Reads a channel 0-15 from the ADC
uint16_t adcRead(uint32_t channel);
// adcSetChannels - Sets the channels sampled by the ADC (ADC must be off!)
void adcSetChannels(uint32_t num, ...);
// i2cSetAddress - Sets the Maple's I2C address
void i2cSetAddress(uint8_t addr);
// ioMultiSetDirection - Configures the specified pins in the bit mask to the given type
void ioMultiSetDirection(GPIOPort* port, uint16_t pinMask, uint32_t type);
// ioSetDirection - Configures the pin as an input or output with a variety of settings
void ioSetDirection(GPIOPort* port, uint32_t pin, uint32_t type);

// ---- Formatted character I/O routines ----

// Variadic macro variations of formatted print, which are only useful in a handful of cases
// vfprintf - Prints the formatted string to the specified output stream
int vfprintf(FILE *stream, const char *formatString, va_list arguments);
// vsprintf - Prints the formatted string to a string buffer
int vsprintf(char *buffer, const char *formatString, va_list arguments);
// vsnprintf - Prints the formatted string to a string buffer with the given length limit
int vsnprintf(char *buffer, size_t limit, const char *formatString, va_list arguments);

// ---- Advanced real-time scheduler routines ----

// Items can be placed on the front or the back of a queue
#define	QUEUE_TO_BACK 0
#define	QUEUE_TO_FRONT 1

// queueCreate - Creates a queue of the specified length, which holds items of the given size
MessageQueue queueCreate(const uint32_t queueLength, const size_t itemSize);
// queueDelete - Deletes a queue
void queueDelete(MessageQueue queue);
// _queueReceive - Retrieves the front item from the queue; if peek is false, removes it as well
bool _queueReceive(MessageQueue q, void *buffer, const clock_t timeout, const bool peek);
// queueReceiveISR - A version of queueReceive() that can be used from an ISR
bool queueReceiveISR(MessageQueue q, void *buffer, bool *taskWoken);
// queueSend - Inserts an item into the queue at the specified position
bool queueSend(MessageQueue q, const void *item, const clock_t timeout,
	const uint32_t position);
// queueSendISR - A version of queueSend() that can be used from an ISR
bool queueSendISR(MessageQueue q, const void *item, bool *contextSwitch,
	const uint32_t position);
// queueSize - Determines the size of a queue
uint32_t queueSize(const MessageQueue queue);

// End C++ extern to C
#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#ifndef USB_CDCACM_H_
#define USB_CDCACM_H_

// Standard library includes
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Virtual COM port configuration/control endpoint (set baud rate, stop bits...)
#define VCOM_CTRL_EPNUM 0
#define VCOM_CTRL_RX_ADDR 0x40
#define VCOM_CTRL_TX_ADDR 0x80
#define VCOM_CTRL_EPSIZE 0x40

// Virtual COM port data transmit endpoint
#define VCOM_TX_ENDP 1
#define VCOM_TX_ADDR 0xC0
#define VCOM_TX_EPSIZE 0x40

// Virtual COM port notification endpoint
#define VCOM_NOTIFICATION_ENDP 2
#define VCOM_NOTIFICATION_ADDR 0x100
#define VCOM_NOTIFICATION_EPSIZE 0x40

// Virtual COM port receive endpoint
#define VCOM_RX_ENDP 3
#define VCOM_RX_ADDR 0x110
#define VCOM_RX_EPSIZE 0x40

// Virtual COM port configuration/control requests
#define SET_LINE_CODING 0x20
#define GET_LINE_CODING 0x21
#define SET_COMM_FEATURE 0x02
#define SET_CONTROL_LINE_STATE 0x22
#define CONTROL_LINE_DTR 0x01
#define CONTROL_LINE_RTS 0x02

// # of endpoints implemented (including endpoint zero)
#define USB_NUM_EP 4
// # of configurations implemented
#define USB_NUM_CONFIG 1
#define USB_MAX_PACKET 0x40

// Writes a character to the USB VCOM port
void usbAcmPut(char ch);
// Reads a character from the USB VCOM port
char usbAcmGet();
// Returns number of bytes available for reading from RX buffer
uint32_t usbAcmCount();
// Returns true if DTR bit is asserted
bool usbAcmIsDTR();
// Returns true if DTR bit is asserted
bool usbAcmIsRTS();
// Initialize USB library
void usbInit();
// Returns true if USB is connected to the host
bool usbIsConnected();

#ifdef __cplusplus
}
#endif

#endif

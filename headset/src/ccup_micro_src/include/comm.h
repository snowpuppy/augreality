/*
 * comm.h - SPI slave driver and USART communication drivers for STM32F4xx chip
 *
 * Used for Raspberry PI communication and the GPS/XBee/Spare headers.
 */

#ifndef COMM_H_
#define COMM_H_

#include <stdbool.h>
#include <stdint.h>

// Constants for the serial function to specify the port to use
#define SERIAL_PORT_GPS 0x00
#define SERIAL_PORT_SPARE 0x01
#define SERIAL_PORT_USB 0x02

// Size of buffer, in bytes, for the communication ring buffers
// Can be at most 256 before overflowing the space in a byte; must be a power of two
#define COMM_BUFFER_SIZE 0x80

// Structure containing a ring buffer
typedef struct {
	uint16_t head;
	uint16_t tail;
	char buffer[COMM_BUFFER_SIZE];
} RingBuffer_TypeDef;

/**
 * Clears the serial buffers.
 */
void serialBufferClear(void);
/**
 * Determine the number of available characters on the specified port.
 *
 * @param port the port to count
 * @return the number of characters available
 */
uint32_t serialBufferCount(uint32_t port);
/**
 * Initialize all USART peripherals used.
 */
void serialInit();
/**
 * Reads a byte from the specified serial port, blocking until one is available.
 *
 * @param port the port to read
 * @return the byte read
 */
uint8_t serialReadByte(uint32_t port);
/**
 * Writes a byte to the specified serial port.
 *
 * @param port the port to write
 * @param data the data byte to write
 */
void serialWriteByte(uint32_t port, uint8_t data);
/**
 * Writes multiple bytes to the specified serial port.
 *
 * @param port the port to write
 * @param data a pointer to the data byte(s) to write
 * @param count the number of data values to write
 */
void serialWriteBytes(uint32_t port, uint8_t *data, uint32_t count);

#endif

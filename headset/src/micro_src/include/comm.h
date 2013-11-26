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
#define SERIAL_PORT_XBEE 0x01
#define SERIAL_PORT_SPARE 0x02
#define SERIAL_PORT_USB 0x03

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
/**
 * Initializes the SPI peripheral as slave mode.
 */
void spiInit();
/**
 * Pushes a byte to the SPI ring buffer, which will be shifted out to the master device on the
 * NEXT (not the current!) SPI transaction.
 *
 * @param data the data byte to queue for writing
 */
void spiWriteByte(uint8_t data);
/**
 * Pushes multiple bytes to the SPI ring buffer, which will be shifted out to the master device
 * one by one on subsequent (not the current!) SPI transactions.
 *
 * @param data a pointer to the data byte(s) to queue for writing
 * @param count the number of data values to queue for writing
 */
void spiWriteBytes(uint8_t *data, uint32_t count);

/**
 * User callback invoked whenever a byte is RXed over SPI.
 *
 * @param data the data byte received over SPI
 */
extern void spiReceivedByte(uint8_t data);

/**
* @brief Empties the spi buffer of characters by setting the
*       head equal to the tail and writing the first bit to zero.
*/
void emptySpiBuffer(void);

#endif

/*
 * spi.h - SPI slave driver for STM32F4xx chip for Raspberry PI communication
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdbool.h>
#include <stdint.h>

// spiInit - Initialize SPI peripheral as slave mode
void spiInit();
// spiWriteByte - Pushes a byte to the SPI ring buffer, which will be shifted out to the master
// device on the NEXT (not the current!) SPI transaction
void spiWriteByte(uint8_t data);
// spiWriteBytes - Pushes multiple bytes to the SPI ring buffer, which will be shifted out to
// the master device one by one on subsequent (not the current!) SPI transactions
void spiWriteBytes(uint8_t *data, uint32_t count);

// User callback invoked whenever a byte is RXed over SPI
extern void spiReceivedByte(uint8_t data);

#endif

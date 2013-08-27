/*
 * spi.h - SPI communication definitions for hardware SPI 1 and 2 (3 not usable)
 *
 * This is meant for the LeafLabs Maple, NOT for the PROS implementation on the VEX Cortex.
 * The Cortex has no hardware SPI peripherals on user accessible pins; use softspi instead.
 *
 * Do NOT move these to the periph.h file, as they are Maple-specific.
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

#ifndef SPI_H_
#define SPI_H_

#include <API.h>

// SPI flags
// 8-bit word size flag
#define SPI_DATA_8 ((uint16_t)0)
// 16-bit word size flag
#define SPI_DATA_16 ((uint16_t)0x0800)
// Clock polarity 0
#define SPI_CPOL_0 ((uint16_t)0)
// Clock polarity 1
#define SPI_CPOL_1 ((uint16_t)0x0002)
// Clock phase 0
#define SPI_CPHA_0 ((uint16_t)0)
// Clock phase 1
#define SPI_CPHA_1 ((uint16_t)0x0001)
// Most significant bit sent first
#define SPI_MSB_FIRST ((uint16_t)0)
// Least significant bit sent first
#define SPI_LSB_FIRST ((uint16_t)0x0080)

// These functions do not send chip select commands - use digitalWrite() if necessary

// spiInit - Initialize the SPI peripheral using the specified clock rate in KHz and SPI flags
// Clock speed will be rounded down to the next lowest attainable speed
void spiInit(uint32_t spi, uint32_t clock, uint16_t flags);
// spiShutdown - Shuts down and disables the specified SPI
void spiShutdown(uint32_t spi);
// spiWrite - Writes the given value out over SPI and ignores the value read in
void spiWrite(uint32_t spi, uint16_t value);
// spiWriteBytes - Writes multiple 8-bit values out over SPI (only use in 8-bit mode)
void spiWriteBytes(uint32_t spi, uint8_t *value, uint16_t length);
// spiWriteRead - Reads a value over SPI while writing the specified value
uint16_t spiWriteRead(uint32_t spi, uint16_t value);
// spiWriteReadBytes - Reads multiple 8-bit values over SPI while writing the specified values
void spiWriteReadBytes(uint32_t spi, uint8_t *inValue, uint8_t *outValue, uint16_t length);

#endif

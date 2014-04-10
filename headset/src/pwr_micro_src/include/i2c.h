/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * i2c.h - I2C (TWI) implementation for STM32F1/3/4xx and STM32L1xx chips
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Initializes the I2C peripheral. The STM32L1 address is set to 0x10.
 */
void i2cInit();
/**
 * Reads the specified number of data bytes from the specified address.
 *
 * @param addr the target address to read
 * @param data the buffer into which data will be placed
 * @param count the number of bytes to read
 * @return whether the operation completed successfully
 */
bool i2cRead(uint8_t addr, uint8_t *data, uint32_t count);
/**
 * Reads the specified number of data bytes from a given register on the specified address,
 * using the I2C "Repeated Start" convention.
 *
 * @param addr the target address to read
 * @param reg the register address on the target to read
 * @param data the buffer into which data will be placed
 * @param count the number of bytes to read
 * @return whether the operation completed successfully
 */
bool i2cReadRegister(uint8_t addr, uint8_t reg, uint8_t *value, uint32_t count);
/**
 * Writes the specified number of data bytes to the specified address.
 *
 * @param addr the target address to write
 * @param data the buffer where data is stored
 * @param count the number of bytes to write
 * @return whether the operation completed successfully
 */
bool i2cWrite(uint8_t addr, uint8_t *data, uint32_t count);
/**
 * Writes a data byte to the specified register on the specified address.
 *
 * @param addr the target address to write
 * @param reg the register address on the target to write
 * @param value the byte to write
 * @return whether the operation completed successfully
 */
bool i2cWriteRegister(uint8_t addr, uint8_t reg, uint8_t value);

#endif

/*
 * i2c.h
 *
 *  Created on: Sep 14, 2013
 *      Author: Stephen
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include <stdint.h>

void i2cInit();
// i2cRead - Reads the specified number of data bytes from the specified address
bool i2cRead(uint8_t addr, uint8_t *data, uint16_t count);
// i2cReadRegister - Reads the specified amount of data from the given register address on
// the specified I2C address
bool i2cReadRegister(uint8_t addr, uint8_t reg, uint8_t *value, uint16_t count);
// i2cSetAddress - Sets the Cortex's I2C address; must be used when I2C is off
void i2cSetAddress(uint8_t addr);
// i2cWrite - Writes the specified number of data bytes to the specified address
bool i2cWrite(uint8_t addr, uint8_t *data, uint16_t count);
// i2cWriteRegister - Writes the specified data to a register on the specified I2C address
bool i2cWriteRegister(uint8_t addr, uint8_t reg, uint16_t value);

#endif

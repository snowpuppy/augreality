/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * i2c.c - I2C implementation
 */

#include "main.h"

// I2C statuses
#define I2C_READ_BIT ((uint8_t)0x01)
#define I2C_STATUS_ERR ((uint8_t)0x01)
#define I2C_STATUS_RESTART ((uint8_t)0x02)
#define I2C_STATUS_NOSTOP ((uint8_t)0x04)

// I2C state flags for the interrupt driver
typedef struct {
	// Up or down?
	volatile uint8_t status;
	// Target I2C address
	volatile uint8_t address;
	// Byte count left
	volatile uint16_t count;
	// Byte buffer to use
	volatile uint8_t *buffer;
} I2CStatus_TypeDef;

// I2C status to use
volatile I2CStatus_TypeDef i2cState;

// I2C status flag initialization
void i2cInit() {
	GPIO_InitTypeDef gs;
	I2C_InitTypeDef is;
	// I2C clocks on
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	__DSB();
	gs.GPIO_Mode = GPIO_Mode_AF;
	gs.GPIO_OType = GPIO_OType_OD;
	gs.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gs.GPIO_Speed = GPIO_Speed_40MHz;
	gs.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &gs);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
	// Reset I2C
	I2C2->CR1 = I2C_CR1_SWRST;
	for (uint32_t delay = 128; delay; delay--) asm volatile("\n\t");
	I2C2->CR1 = 0;
	// Configure I2C
	is.I2C_ClockSpeed = 400000;
	is.I2C_Mode = I2C_Mode_I2C;
	is.I2C_DutyCycle = I2C_DutyCycle_16_9;
	is.I2C_OwnAddress1 = 0x10;
	is.I2C_Ack = I2C_Ack_Disable;
	is.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C2, &is);
	// Turn I2C on
	I2C2->CR1 |= I2C_CR1_PE;
	// Turn on error interrupt (event interrupt turned on when needed)
	I2C2->CR2 |= I2C_CR2_ITERREN;
	i2cState.status = 0;
	// IRQs on
	NVIC_EnableIRQ(I2C2_ER_IRQn);
	NVIC_EnableIRQ(I2C2_EV_IRQn);
	NVIC_SetPriority(I2C2_ER_IRQn, 1);
	NVIC_SetPriority(I2C2_EV_IRQn, 1);
}

// _i2cDisableInt - Common I2C end routine to disable interrupts
static inline void _i2cDisableInt() {
	I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
}

// _i2cEnableInt - Common I2C start routine to enable interrupts
static inline void _i2cEnableInt() {
	I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN;
}

// _i2cRead - Reads the specified number of data bytes from the specified address
static bool _i2cRead(uint8_t addr, uint8_t *data, uint16_t count) {
	volatile I2CStatus_TypeDef *state = &i2cState;
	__disable_irq();
	{
		// Set the I2C direction to reception, set LSB to receive properly
		state->address = (addr << 1) | I2C_READ_BIT;
		// Set up buffers
		state->buffer = data;
		state->count = count;
		// Send START condition
		I2C2->CR1 |= I2C_CR1_ACK | I2C_CR1_START;
		state->status = 0;
	}
	__enable_irq();
	// Wait until START is reset (transmission begins)
	do {
		SLEEP();
		if (state->status & I2C_STATUS_ERR)
			return false;
	} while (!(I2C2->SR1 & I2C_SR1_SB));
	// Enable I2C interrupts
	_i2cEnableInt();
	// Wait until BUSY flag is reset (until a STOP is generated)
	do {
		SLEEP();
		// Error occurred?
		if (state->status & I2C_STATUS_ERR)
			return false;
	} while (I2C2->SR2 & I2C_SR2_BUSY);
	return true;
}

// Writes data to the I2C interface, observing the NOSTOP flag for repeated-start generation
static bool _i2cWrite(uint8_t addr, uint8_t *data, uint16_t count) {
	uint8_t status;
	volatile I2CStatus_TypeDef *state = &i2cState;
	__disable_irq();
	{
		// Set the I2C direction to transmission, clear LSB to transmit properly
		state->address = addr << 1;
		// Set up buffers
		state->buffer = data;
		state->count = count;
		// Send START condition
		I2C2->CR1 |= I2C_CR1_START | I2C_CR1_ACK;
	}
	__enable_irq();
	// Wait until START is reset (transmission begins)
	do {
		SLEEP();
		if (state->status & I2C_STATUS_ERR)
			return false;
	} while (!(I2C2->SR1 & I2C_SR1_SB));
	// Enable I2C interrupts
	_i2cEnableInt();
	// Wait until BUSY flag is reset (until a STOP is generated) or repeated-start bit set
	do {
		SLEEP();
		status = state->status;
		// Error occurred?
		if (status & I2C_STATUS_ERR)
			return false;
		// Break condition so that repeated start works
		if (status & I2C_STATUS_RESTART)
			break;
	} while (I2C2->SR2 & I2C_SR2_BUSY);
	return true;
}

// i2cRead - Reads the specified number of data bytes from the specified address
bool i2cRead(uint8_t addr, uint8_t *data, uint32_t count) {
	return _i2cRead(addr, data, count);
}

// i2cReadRegister - Reads the specified amount of data from the given register address on
// the specified I2C address
bool i2cReadRegister(uint8_t addr, uint8_t reg, uint8_t *value, uint32_t count) {
	i2cState.status = I2C_STATUS_NOSTOP;
	// Write out the location we want and read in data
	return _i2cWrite(addr, &reg, 1) && _i2cRead(addr, value, count);
}

// i2cWrite - Writes the specified number of data bytes to the specified address
bool i2cWrite(uint8_t addr, uint8_t *data, uint32_t count) {
	i2cState.status = 0;
	return _i2cWrite(addr, data, count);
}

// i2cWriteRegister - Writes the specified data to a register on the specified I2C address
bool i2cWriteRegister(uint8_t addr, uint8_t reg, uint8_t value) {
	uint8_t data[2];
	// Write register, value
	data[0] = reg;
	data[1] = value;
	// Write information
	return i2cWrite(addr, data, 2);
}

// I2C2 event interrupt
void IRQ I2C2_EV_IRQHandler(void) {
	uint16_t sr1 = I2C2->SR1;
	volatile I2CStatus_TypeDef *state = &i2cState;
	// Read SR2
	I2C2->SR2;
	if (sr1 & I2C_SR1_SB)
		// If SB = 1, START sent (EV5)
		I2C2->DR = state->address;
	else if (sr1 & I2C_SR1_ADDR) {
		// If ADDR = 1, address sent (EV6)
		uint8_t count = state->count;
		// Write the first data when transmitting (LSB clear)
		if (!(state->address & I2C_READ_BIT)) {
			// Write the first data in the data register
			I2C2->DR = *((state->buffer)++);
			count--;
			state->count = count;
			// If no further data, disable buffer interrupt so as not to have a TXE interrupt
			if (count == 0)
				I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
		} else if (count == 1)
			// EV6_1, used for single byte reception. Disable ACK and program STOP
			I2C2->CR1 = (I2C2->CR1 & ~I2C_CR1_ACK) | I2C_CR1_STOP;
	} else if (sr1 & I2C_SR1_TXE) {
		uint8_t count = state->count;
		// TXE set, transmit data
		if (sr1 & I2C_SR1_BTF) {
			uint8_t status = state->status;
			// EV8_2, both BTF and TXE are set
			if (status & I2C_STATUS_NOSTOP)
				// Prepare for repeated START bit
				state->status = (status & ~I2C_STATUS_NOSTOP) | I2C_STATUS_RESTART;
			else
				// Send STOP
				I2C2->CR1 |= I2C_CR1_STOP;
			// Disable event interrupt to avoid second BTF interrupt
			_i2cDisableInt();
		} else if (count > 0) {
			// Write data
			I2C2->DR = *((state->buffer)++);
			count--;
			state->count = count;
			// If no data left, avoid last TXE interrupt
			if (count == 0)
				I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
		}
	} else if (sr1 & I2C_SR1_RXNE) {
		uint8_t count = state->count;
		// RXNE set, read in data
		*((state->buffer)++) = I2C2->DR;
		count--;
		state->count = count;
		// If one byte left, disable ACK and send STOP (EV7_1)
		if (count == 1)
			// No ACK, send STOP
			I2C2->CR1 = (I2C2->CR1 & ~I2C_CR1_ACK) | I2C_CR1_STOP;
		else if (count == 0)
			// All done
			_i2cDisableInt();
	}
}

// I2C2 error interrupt
void IRQ I2C2_ER_IRQHandler(void) {
	uint16_t cr, flags = I2C2->SR1;
	// Store interrupt flags, then clear them
	I2C2->SR1 = flags & ~(I2C_SR1_ARLO | I2C_SR1_BERR | I2C_SR1_OVR | I2C_SR1_AF);
	cr = I2C2->CR1;
	// Recover from arbitration lost and bus error via resetting I2C
	if (flags & (I2C_SR1_ARLO | I2C_SR1_BERR | I2C_SR1_OVR)) {
		// Software reset the I2C peripheral
		I2C2->CR1 = cr | I2C_CR1_SWRST;
		// This delay required for I2C to acknowledge the request
		for (uint32_t delay = 32; delay; delay--) asm volatile("");
		// Clear software reset
		I2C2->CR1 = cr;
	} else
		// Send STOP bit
		I2C2->CR1 = cr | I2C_CR1_STOP;
	// Let SW know to break out of wait state
	i2cState.status |= I2C_STATUS_ERR;
	// Disable future interrupts
	_i2cDisableInt();
}

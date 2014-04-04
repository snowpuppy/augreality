/*
 * comm.c - USART communication drivers for STM32L1xx chip
 */

#include "usb_cdcacm.h"
#include "main.h"
#include "comm.h"
#include "printf.h"

// Size of buffer, in bytes, for the communication ring buffers
// Can be at most 256 before overflowing the space in a byte; must be a power of two
#define COMM_BUFFER_SIZE 0x40
// Maximum value that the size can have before filling
#define _COMM_MAX (COMM_BUFFER_SIZE - 1)

// Structure containing a ring buffer
typedef struct {
	uint8_t head;
	uint8_t tail;
	char buffer[COMM_BUFFER_SIZE];
} RingBuffer_TypeDef;

// Serial buffers
static RingBuffer_TypeDef serialBufferRX;
static RingBuffer_TypeDef serialBufferTX;

// Checks to see if the ring buffer is full (tail + 1 = head)
static inline uint8_t _isBufferFull(volatile RingBuffer_TypeDef* buffer) {
	return ((buffer->tail + 1) & _COMM_MAX) == buffer->head;
}

// Checks to see if the ring buffer is empty (head = tail)
static inline uint8_t _isBufferEmpty(volatile RingBuffer_TypeDef* buffer) {
	return buffer->head == buffer->tail;
}

// Removes a byte from the head of the given buffer
static char _pullByte(volatile RingBuffer_TypeDef* buffer) {
	uint8_t head = buffer->head; char value;
	value = buffer->buffer[head];
	buffer->head = (head + 1) & _COMM_MAX;
	return value;
}

// Queues a byte onto the tail of the given buffer
static void _queueByte(volatile RingBuffer_TypeDef* buffer, char value) {
	uint8_t tail = buffer->tail;
	buffer->buffer[tail] = value;
	buffer->tail = (tail + 1) & _COMM_MAX;
}

/**
 * Determine the number of available characters on the specified port.
 *
 * @param port the port to count
 * @return the number of characters available
 */
uint32_t serialBufferCount(uint32_t port) {
	uint32_t head, tail;
	if (port == SERIAL_PORT_USB)
		return usbAcmCount();
	if (port > SERIAL_PORT_DEBUG)
		return 0U;
	// Disable interrupts to read
	__disable_irq();
	// Pull the head and tail pointers
	head = (uint32_t)serialBufferRX.head;
	tail = (uint32_t)serialBufferRX.tail;
	__enable_irq();
	// Calculate difference modulo size
	return (tail - head) & (uint32_t)_COMM_MAX;
}

/**
 * Initialize all USART peripherals used.
 */
void serialInit() {
	USART_InitTypeDef uart;
	GPIO_InitTypeDef gpio;
	// Prime the buffers to empty
	serialBufferTX.head = 0;
	serialBufferTX.tail = 0;
	serialBufferRX.head = 0;
	serialBufferRX.tail = 0;
	// Enable all peripheral clocks required
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	// Set up the pins for USART
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio);
	// Set up pin sources
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	// Initialize Spare to 115200 8N1
	uart.USART_BaudRate = 115200;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &uart);
	// Enable all RXNE interrupts
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	// NVIC interrupts enable
	NVIC_SetPriority(USART1_IRQn, 3);
	NVIC_EnableIRQ(USART1_IRQn);
	// USARTs on
	USART1->CR1 |= USART_CR1_UE;
}

/**
 * Reads a byte from the specified serial port, blocking until one is available.
 *
 * @param port the port to read
 * @return the byte read
 */
uint8_t serialReadByte(uint32_t port) {
	if (port == SERIAL_PORT_USB)
		return usbAcmGet();
	if (port > SERIAL_PORT_DEBUG)
		return 0U;
	// Wait for data
	while (_isBufferEmpty(&serialBufferRX)) SLEEP();
	// Return what we got
	return (uint8_t)_pullByte(&serialBufferRX);
}

/**
 * Writes a byte to the specified serial port.
 *
 * @param port the port to write
 * @param data the data byte to write
 */
void serialWriteByte(uint32_t port, uint8_t data) {
	if (port == SERIAL_PORT_USB)
		// Stuff OS
		usbAcmPut(data);
	else if (port == SERIAL_PORT_DEBUG) {
		// Wait for buffer space
		while (_isBufferFull(&serialBufferTX)) SLEEP();
		_queueByte(&serialBufferTX, (char)data);
		// Enable transmit interrupts and start transmitting if not already doing so
		USART1->CR1 |= USART_CR1_TXEIE;
	}
}

// Stub to redirect outputs from printf()
int fputc(int c, FILE *stream) {
	// Pointer to integer without cast intentional
	serialWriteByte((uint32_t)stream, (uint8_t)(uint32_t)c);
	return c;
}

// Stub to redirect inputs
int fgetc(FILE *stream) {
	// Pointer to integer without cast intentional
	return (char)serialReadByte((uint32_t)stream);
}

/**
 * Writes multiple bytes to the specified serial port.
 *
 * @param port the port to write
 * @param data a pointer to the data byte(s) to write
 * @param count the number of data values to write
 */
void serialWriteBytes(uint32_t port, uint8_t *data, uint32_t count) {
	while (count > 0) {
		serialWriteByte(port, *data++);
		count--;
	}
}

/**
 * USART 1 interrupt handler for TXE and RXNE interrupts [DEBUG]
 */
void __attribute__ ((interrupt("IRQ"))) USART1_IRQHandler() {
	if (USART1->SR & USART_SR_RXNE) {
		// Shut off the alarm clock
		char data = (char)(uint8_t)USART1->DR;
		// If space, put in the RX buffer
		if (!_isBufferFull(&serialBufferRX))
			_queueByte(&serialBufferRX, data);
	} else if (USART1->SR & USART_SR_TXE) {
		// TX empty, stuff the byte if possible
		if (_isBufferEmpty(&serialBufferTX))
			USART1->CR1 &= ~USART_CR1_TXEIE;
		else
			// AVOID SIGN EXTENSION
			USART1->DR = (uint16_t)(uint8_t)_pullByte(&serialBufferTX);
	}
}

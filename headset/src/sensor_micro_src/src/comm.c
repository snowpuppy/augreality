/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * comm.c - USART communication drivers for STM32F4xx chip
 * Used for the GPS/Spare headers.
 */

#include "usb.h"
#include "main.h"
#include "comm.h"
#include "printf.h"

// Maximum value that the size can have before filling
#define _COMM_MAX (COMM_BUFFER_SIZE - 1)
#define _USB_MAX (APP_RX_DATA_SIZE - 1)

// Serial buffers
static RingBuffer_TypeDef serialBufferRX[3] = {{0}};
static RingBuffer_TypeDef serialBufferTX[2] = {{0}};

// Checks to see if the ring buffer is full (tail + 1 = head)
static inline bool _isBufferFull(volatile RingBuffer_TypeDef* buffer) {
	return ((buffer->tail + 1) & _COMM_MAX) == buffer->head;
}

// Checks to see if the ring buffer is empty (head = tail)
static inline bool _isBufferEmpty(volatile RingBuffer_TypeDef* buffer) {
	return buffer->head == buffer->tail;
}

// Removes a byte from the head of the given buffer
static char _pullByte(volatile RingBuffer_TypeDef* buffer) {
	uint16_t head = buffer->head; char value;
	value = buffer->buffer[head];
	buffer->head = (head + 1) & _COMM_MAX;
	return value;
}

// Queues a byte onto the tail of the given buffer
static void _queueByte(volatile RingBuffer_TypeDef* buffer, char value) {
	uint16_t tail = buffer->tail;
	buffer->buffer[tail] = value;
	buffer->tail = (tail + 1) & _COMM_MAX;
}

// Data to be sent over USB IN endpoint (ring buffer)
extern uint8_t usbCdcRxBuffer[APP_RX_DATA_SIZE];
extern volatile uint32_t usbCdcRxBufferIn;
extern volatile uint32_t usbCdcRxBufferOut;

/**
 * Queues a character onto the VCP buffer.
 * This library is not a true high-performance system; sending lots of data to the USB port
 * will cause excess bytes to be dropped if the bytes cannot be emptied from the USB ring buffer
 * (128 bytes) as fast as they are put in. The USB output buffer is somewhat bigger, as defined
 * in usb.h, but can still be filled at maximum baud rate before the TX hits.
 *
 * @param c the character to append
 */
static void usbVCPTx(uint8_t c) {
	uint32_t ptr = usbCdcRxBufferIn;
	// Wait for space
	if (usbVCPConnected() && ((ptr - usbCdcRxBufferOut) & _USB_MAX) != _USB_MAX) {
		// Queue the byte
		usbCdcRxBuffer[ptr] = c;
		usbCdcRxBufferIn = (ptr + 1) & _USB_MAX;
	}
}

/**
 * Clears the serial buffers.
 */
void serialBufferClear(void) {
	// Prime the buffers to empty
	for (uint32_t i = 0; i < 2; i++) {
		serialBufferTX[i].head = 0;
		serialBufferTX[i].tail = 0;
		serialBufferRX[i].head = 0;
		serialBufferRX[i].tail = 0;
	}
}

/**
 * Determine the number of available characters on the specified port.
 *
 * @param port the port to count
 * @return the number of characters available
 */
uint32_t serialBufferCount(uint32_t port) {
	uint32_t head, tail;
	if (port > SERIAL_PORT_USB) return 0;
	// Disable interrupts to read
	__disable_irq();
	// Pull the head and tail pointers
	head = (uint32_t)serialBufferRX[port].head;
	tail = (uint32_t)serialBufferRX[port].tail;
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
	serialBufferClear();
	// Enable all peripheral clocks required
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	// Set up the pins for each USART
	// SPARE
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	// GPS
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOC, &gpio);
	// Set up pin sources
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
	// Initialize GPS to 38400 8N1
	uart.USART_BaudRate = 38400;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART6, &uart);
	// Initialize Spare to 115200 8N1
	uart.USART_BaudRate = 115200;
	USART_Init(USART1, &uart);
	// Enable all RXNE interrupts
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	// NVIC interrupts enable
	NVIC_SetPriority(USART1_IRQn, 5);
	NVIC_SetPriority(USART6_IRQn, 5);
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_EnableIRQ(USART6_IRQn);
	// USARTs on
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART6, ENABLE);
}

/**
 * Reads a byte from the specified serial port, blocking until one is available.
 *
 * @param port the port to read
 * @return the byte read
 */
uint8_t serialReadByte(uint32_t port) {
	RingBuffer_TypeDef *buf;
	if (port > SERIAL_PORT_USB) return 0U;
	buf = &serialBufferRX[port];
	// Wait for data
	while (_isBufferEmpty(buf)) __WFI();
	// Return what we got
	return (uint8_t)_pullByte(buf);
}

/**
 * Writes a byte to the specified serial port.
 *
 * @param port the port to write
 * @param data the data byte to write
 */
void serialWriteByte(uint32_t port, uint8_t data) {
	USART_TypeDef *usart;
	RingBuffer_TypeDef *buf;
	switch (port) {
	case SERIAL_PORT_GPS:
		// Stuff GPS
		usart = USART6;
		break;
	case SERIAL_PORT_SPARE:
		// Stuff Spare
		usart = USART1;
		break;
	case SERIAL_PORT_USB:
		// Stuff OS
		usbVCPTx(data);
		return;
	default:
		return;
	}
	// Wait for buffer space
	buf = &serialBufferTX[port];
	while (_isBufferFull(buf)) __WFI();
	_queueByte(buf, (char)data);
	// Enable transmit interrupts and start transmitting if not already doing so
	usart->CR1 |= USART_CR1_TXEIE;
}

// Stub to redirect count requests to serialBufferCount
int fcount(FILE *stream) {
	// Pointer to integer without cast intentional
	return (int)serialBufferCount((uint32_t)stream);
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
 * USART 1 interrupt handler for TXE and RXNE interrupts [SPARE]
 */
void __attribute__ ((interrupt("IRQ"))) USART1_IRQHandler() {
	RingBuffer_TypeDef *buf;
	if (USART1->SR & USART_SR_RXNE) {
		// Shut off the alarm clock
		char data = (char)(uint8_t)USART1->DR;
		buf = &serialBufferRX[SERIAL_PORT_SPARE];
		// If space, put in the RX buffer
		if (!_isBufferFull(buf))
			_queueByte(buf, data);
	} else if (USART1->SR & USART_SR_TXE) {
		buf = &serialBufferTX[SERIAL_PORT_SPARE];
		// TX empty, stuff the byte if possible
		if (_isBufferEmpty(buf))
			USART1->CR1 &= ~USART_CR1_TXEIE;
		else
			// AVOID SIGN EXTENSION
			USART1->DR = (uint16_t)(uint8_t)_pullByte(buf);
	}
}

/**
 * USART 6 interrupt handler for TXE and RXNE interrupts [GPS]
 */
void __attribute__ ((interrupt("IRQ"))) USART6_IRQHandler() {
	RingBuffer_TypeDef *buf;
	if (USART6->SR & USART_SR_RXNE) {
		// Shut off the alarm clock
		char data = (char)(uint8_t)USART6->DR;
		buf = &serialBufferRX[SERIAL_PORT_GPS];
		// If space, put in the RX buffer
		if (!_isBufferFull(buf))
			_queueByte(buf, data);
	} else if (USART6->SR & USART_SR_TXE) {
		buf = &serialBufferTX[SERIAL_PORT_GPS];
		// TX empty, stuff the byte if possible
		if (_isBufferEmpty(buf))
			USART6->CR1 &= ~USART_CR1_TXEIE;
		else
			// AVOID SIGN EXTENSION
			USART6->DR = (uint16_t)(uint8_t)_pullByte(buf);
	}
}

/**
 * Receives up to size bytes into buffer. This function is direct and much, much faster than
 * looping on fgetc(), therefore should be used when receiving lots of data over USB VCP.
 *
 * @param buffer the buffer to store collected bytes
 * @param size the maximum number of bytes to store
 * @return the number of bytes actually stored
 */
uint32_t usbVCPRead(uint8_t *buffer, uint32_t size) {
	RingBuffer_TypeDef *rb = &serialBufferRX[SERIAL_PORT_USB];
	uint32_t count = 0;
	// This is basically a fast loop on buffer empty without the switch-case and indirection
	while (count < size && !_isBufferEmpty(rb)) {
		*buffer++ = (uint8_t)_pullByte(rb);
		count++;
	}
	return count;
}

/**
 * Writes up to size bytes of data from buffer. This function is direct and much, much faster
 * than looping on fputc(), therefore should be used when sending lots of data over USB VCP.
 *
 * @param buffer the buffer to send
 * @param size the number of bytes to send
 * @return the number of bytes actually sent
 */
uint32_t usbVCPWrite(uint8_t *buffer, uint32_t size) {
	uint32_t ptr = usbCdcRxBufferIn, count = 0;
	// Check for space
	while (count < size && ((ptr - usbCdcRxBufferOut) & _USB_MAX) != _USB_MAX) {
		// Queue the byte
		usbCdcRxBuffer[ptr] = *buffer++;
		usbCdcRxBufferIn = ptr = (ptr + 1) & _USB_MAX;
		count++;
	}
	return count;
}

/**
 * Queue bytes until no more can be queued.
 *
 * @param buf the bytes to be queued
 * @param len how many bytes to queue
 * @return always true, since there is nothing better to do with the trashed bytes
 */
bool usbVCPRx(uint8_t* buf, uint32_t len) {
	RingBuffer_TypeDef *rb = &serialBufferRX[SERIAL_PORT_USB];
	while (len-- && !_isBufferFull(rb))
		_queueByte(rb, (char)*buf++);
	return true;
}

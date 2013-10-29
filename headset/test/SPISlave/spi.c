#include "main.h"
#include "spi.h"

// Size of buffer, in bytes, for the SPI ring buffers
// Can be at most 256 before overflowing the space in a byte; must be a power of two
#define SPI_BUFFER_SIZE 0x40
// Maximum value that the size can have before filling
#define _SPI_MAX (SPI_BUFFER_SIZE - 1)

// Structure containing a ring buffer
typedef struct {
	uint8_t head;
	uint8_t tail;
	char buffer[SPI_BUFFER_SIZE];
} RingBuffer_TypeDef;

// SPI buffer
static RingBuffer_TypeDef spiBuffer;

// Checks to see if the ring buffer is empty (head = tail)
static inline uint8_t _isBufferEmpty(volatile RingBuffer_TypeDef* buffer) {
	return buffer->head == buffer->tail;
}

// Removes a byte from the head of the given buffer
static char _pullByte(volatile RingBuffer_TypeDef* buffer) {
	uint8_t head = buffer->head; char value;
	value = buffer->buffer[head];
	buffer->head = (head + 1) & _SPI_MAX;
	return value;
}

// Queues a byte onto the tail of the given buffer
static void _queueByte(volatile RingBuffer_TypeDef* buffer, char value) {
	uint8_t tail = buffer->tail;
	buffer->buffer[tail] = value;
	buffer->tail = (tail + 1) & _SPI_MAX;
}

// spiInit - Initialize SPI peripheral as slave mode
void spiInit() {
	SPI_InitTypeDef spi;
	GPIO_InitTypeDef gpio;
	// Turn on SPI clock and Port C clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// Configure SPI pins appropriately
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio);
	// Allow SPI3 to take over the pins
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource13, GPIO_AF_SPI3);
	// Does not matter, this is slave mode
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	// CPHA 0 CPOL 0 (Mode 0)
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CRCPolynomial = 1;
	// MSB first !?, 8 bits, bidirectional
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	// Slave mode with the hardware !SS! pin in use
	spi.SPI_Mode = SPI_Mode_Slave;
	spi.SPI_NSS = SPI_NSS_Hard;
	SPI_Init(SPI3, &spi);
	// SPI interrupt config
	SPI3->CR2 |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE;
	SPI_Cmd(SPI3, ENABLE);
	// Turn on SPI interrupts
	NVIC_SetPriority(SPI3_IRQn, 2);
	NVIC_EnableIRQ(SPI3_IRQn);
}

// spiWriteByte - Pushes a byte to the SPI ring buffer, which will be shifted out to the master
// device on the NEXT (not the current!) SPI transaction
void spiWriteByte(uint8_t data) {
	// DO NOT WAIT FOR SPACE! This is probably called in an ISR which will not take well to WFI
	_queueByte(&spiBuffer, (char)data);
	SPI3->CR2 |= SPI_CR2_TXEIE;
}

// spiWriteBytes - Pushes multiple bytes to the SPI ring buffer, which will be shifted out to
// the master device one by one on subsequent (not the current!) SPI transactions
void spiWriteBytes(uint8_t *data, uint32_t count) {
	while (count > 0) {
		spiWriteByte(*data++);
		count--;
	}
}

// SPI 3 interrupt handler
void __attribute__ ((interrupt("IRQ"))) SPI3_IRQHandler() {
	if (SPI3->SR & SPI_SR_RXNE) {
		// Shut off the alarm clock
		uint8_t data = (uint8_t)SPI3->DR;
		// Call the user call back
		spiReceivedByte(data);
	} else if (SPI3->SR & SPI_SR_TXE) {
		// TX empty, stuff the byte if possible
		if (_isBufferEmpty(&spiBuffer))
			SPI3->CR2 &= ~SPI_CR2_TXEIE;
		else
			// AVOID SIGN EXTENSION
			SPI3->DR = (uint16_t)(uint8_t)_pullByte(&spiBuffer);
	}
}

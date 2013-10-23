#include "main.h"
#include "itm.h"
#include "printf.h"

// Size of buffer, in bytes, for the USART ring buffers
// Can be at most 256 before overflowing the space in a byte; must be a power of two
#define USART_BUFFER_SIZE 0x80
// Maximum value that the size can have before filling
#define _USART_MAX (USART_BUFFER_SIZE - 1)

// Structure containing a ring buffer
typedef struct {
	uint8_t head;
	uint8_t tail;
	char buffer[USART_BUFFER_SIZE];
} RingBuffer_TypeDef;

// ITM ring buffer
static volatile RingBuffer_TypeDef itm;

// Checks to see if the ring buffer is empty (head = tail)
static inline uint8_t _isBufferEmpty(volatile RingBuffer_TypeDef* buffer) {
	return buffer->head == buffer->tail;
}

// Checks to see if the ring buffer is full (tail + 1 = head)
static inline uint8_t _isBufferFull(volatile RingBuffer_TypeDef* buffer) {
	return ((buffer->tail + 1) & _USART_MAX) == buffer->head;
}

// Removes a byte from the head of the given buffer
static char _pullByte(volatile RingBuffer_TypeDef* buffer) {
	uint8_t head = buffer->head; char value;
	value = buffer->buffer[head];
	buffer->head = (head + 1) & _USART_MAX;
	return value;
}

// Queues a byte onto the tail of the given buffer
static void _queueByte(volatile RingBuffer_TypeDef* buffer, char value) {
	uint8_t tail = buffer->tail;
	buffer->buffer[tail] = value;
	buffer->tail = (tail + 1) & _USART_MAX;
}

// Send print statements to the PC
int fputc(int c, FILE *stream) {
#ifndef DISABLE_DEBUG
	// Wait for space
	while (ITM->PORT[0].u32 == 0);
	ITM->PORT[0].u8 = (uint8_t)c;
	// Enable interrupt
	TIM7->DIER = TIM_DIER_UIE;
#endif
	return c;
}

// Initialize the integrated trace macrocell for debugging
void itmInit() {
	// XXX When debugging ensure that the core clock runs to allow debugger to connect
	// during Sleep mode; DISABLE THIS LINE USING #define DISABLE_DEBUG WHEN TESTING POWER
	// CONSUMPTION
#ifndef DISABLE_DEBUG
	DBGMCU->CR = DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_TRACE_IOEN;
	// From RM0090 p. 1674
	*((uint32_t *)(ITM_BASE + 0xFB0U)) = 0xC5ACCE55U;
	*((uint32_t *)0xE0040010) = 81;
	ITM->TCR = 0x00010005U;
	ITM->TER = 0x00000001U;
	ITM->TPR = 0x00000001U;
#endif
}

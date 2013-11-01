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
	*((uint32_t *)0xE0040010) = 81U;
	ITM->TCR = 0x00010005U;
	ITM->TER = 0x00000001U;
	ITM->TPR = 0x00000001U;
#endif
}

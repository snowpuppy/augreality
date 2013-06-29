#include "main.h"

// NMI exception handler
void NMI_Handler(void) {
}

// Hard-fault exception handler
void HardFault_Handler(void) {
	while (1) {
	}
}

// Memory mangement exception handler
void MemManage_Handler(void) {
	while (1) {
	}
}

// Bus error exception handler
void BusFault_Handler(void) {
	while (1) {
	}
}

// Usage fault exception handler
void UsageFault_Handler(void) {
	while (1) {
	}
}

// SVCall exception handler
void SVC_Handler(void) {
}

// Debug Monitor exception handler
void DebugMon_Handler(void) {
}

// PendSVC exception handler
void PendSV_Handler(void) {
}

// SysTick event handler
void SysTick_Handler(void) {
	_tick();
}

/*
 * CMSIS Cortex-M3 Device Peripheral Access Layer for Maple RET6
 * Provides definitions and functions to access the low-level registers of the LeafLabs Maple
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

#ifndef MAPLE_H_

// This prevents multiple inclusion
#define MAPLE_H_

// Allow integers to be specified by sign and magnitude (bit-width) independent of definition
#include <stdint.h>
#include <stdbool.h>

// Begin C++ extern to C
#ifdef __cplusplus
extern "C" {
#endif

// Modifier for I/O ports to force direct memory access (avoid register caching)
#define __I volatile
#define __IO volatile

// By default, there are 4 priority bits for the NVIC
#define NVIC_PRIO_BITS 4

// Interrupt types available
typedef enum IRQn {
	// Cortex-M3 default interrupts
	// Non-maskable interrupt
	NonMaskableInt_IRQn = -14,
	// Illegal memory usage or memory management failure (stack overflow, etc.) interrupt
	MemoryManagement_IRQn = -12,
	// Bus fault interrupt
	BusFault_IRQn = -11,
	// Illegal instruction interrupt
	UsageFault_IRQn = -10,
	// SV call interrupt
	SVCall_IRQn = -5,
	// Debug monitor interrupt
	DebugMonitor_IRQn = -4,
	// SV pending interrupt
	PendSV_IRQn = -2,
	// System tick interrupt
	SysTick_IRQn = -1,

	// STM32 specific peripheral interrupts
	// Watchdog interrupt
	WWDG_IRQn = 0,
	// Power supply interrupt
	PVD_IRQn = 1,
	// Tamper interrupt
	TAMPER_IRQn = 2,
	// Real-time clock interrupt
	RTC_IRQn = 3,
	// Flash memory interrupt
	FLASH_IRQn = 4,
	// Reset and clock control interrupt
	RCC_IRQn = 5,
	// External pin interrupts
	EXTI0_IRQn = 6,
	EXTI1_IRQn = 7,
	EXTI2_IRQn = 8,
	EXTI3_IRQn = 9,
	EXTI4_IRQn = 10,

	// HD device-specific interrupts
	// ADC #1/#2 interrupt
	ADC1_2_IRQn = 18,
	// USB high priority/CAN1 TX
	USB_HP_CAN1_TX_IRQn = 19,
	// USB low priority/CAN0 RX
	USB_LP_CAN1_RX0_IRQn = 20,
	// More external pin interrupts
	EXTI9_5_IRQn = 23,
	// Timer #1 break interrupt
	TIM1_BRK_IRQn = 24,
	// Timer #1 updated interrupt
	TIM1_UP_IRQn = 25,
	// Timer #1 trigger interrupt
	TIM1_TRG_COM_IRQn = 26,
	// Timer #1 capture/compare interrupt
	TIM1_CC_IRQn = 27,
	// Timer #2 interrupt
	TIM2_IRQn = 28,
	// Timer #3 interrupt
	TIM3_IRQn = 29,
	// Timer #4 interrupt
	TIM4_IRQn = 30,
	// I2C #1 event interrupt
	I2C1_EV_IRQn = 31,
	// I2C #1 error interrupt
	I2C1_ER_IRQn = 32,
	// I2C #2 event interrupt
	I2C2_EV_IRQn = 33,
	// I2C #2 error interrupt
	I2C2_ER_IRQn = 34,
	// SPI #1 interrupt
	SPI1_IRQn = 35,
	// SPI #2 interrupt
	SPI2_IRQn = 36,
	// USART #1 interrupt
	USART1_IRQn = 37,
	// USART #2 interrupt
	USART2_IRQn = 38,
	// USART #3 interrupt
	USART3_IRQn = 39,
	// Still more external interrupts
	EXTI15_10_IRQn = 40,
#ifdef MAPLE_RET6
	// Timer #8 break interrupt
	TIM8_BRK_IRQn = 43,
	// Timer #8 update interrupt
	TIM8_UP_IRQn = 44,
	// Timer #8 trigger interrupt
	TIM8_TRG_COM_IRQn = 45,
	// Timer #8 capture/compare interrupt
	TIM8_CC_IRQn = 46,
	// ADC #3 interrupt
	ADC3_IRQn = 47,
	// Timer #5 interrupt
	TIM5_IRQn = 50,
	// SPI #3 interrupt
	SPI3_IRQn = 51,
	// UART #4 interrupt
	UART4_IRQn = 52,
	// UART #5 interrupt
	UART5_IRQn = 53,
	// Timer #6 interrupt
	TIM6_IRQn = 54,
	// Timer #7 interrupt
	TIM7_IRQn = 55
#endif
} IRQn_Type;

// ADC register access structure
typedef struct {
	// Status register
	__IO uint32_t SR;
	// Control register #1
	__IO uint32_t CR1;
	// Control register #2
	__IO uint32_t CR2;
	// Sample time register #1
	__IO uint32_t SMPR1;
	// Sample time register #2
	__IO uint32_t SMPR2;
	// Injected channel data offset registers
	__IO uint32_t JOFR1;
	__IO uint32_t JOFR2;
	__IO uint32_t JOFR3;
	__IO uint32_t JOFR4;
	// Analog comparator high threshold register
	__IO uint32_t HTR;
	// Analog comparator low threshold register
	__IO uint32_t LTR;
	// Regular sequence registers
	__IO uint32_t SQR1;
	__IO uint32_t SQR2;
	__IO uint32_t SQR3;
	// Injected sequence register
	__IO uint32_t JSQR;
	// Injected data registers
	__I uint32_t JDR1;
	__I uint32_t JDR2;
	__I uint32_t JDR3;
	__I uint32_t JDR4;
	// Regular data register
	__I uint32_t DR;
} ADC_TypeDef;

// DAC register access structure
typedef struct {
	// Control register
	__IO uint32_t CR;
	// Software trigger register
	__IO uint32_t SWTRIGR;
	// Channel 1 12-bit right aligned data register
	__IO uint32_t DHR12R1;
	// Channel 1 12-bit left aligned data register
	__IO uint32_t DHR12L1;
	// Channel 1 8-bit right aligned data register
	__IO uint32_t DHR8R1;
	// Channel 1 12-bit right aligned data register
	__IO uint32_t DHR12R2;
	// Channel 2 12-bit left aligned data register
	__IO uint32_t DHR12L2;
	// Channel 2 8-bit right aligned data register
	__IO uint32_t DHR8R2;
	// Dual DAC 12-bit right aligned data register
	__IO uint32_t DHR12RD;
	// Dual DAC 12-bit left aligned data register
	__IO uint32_t DHR12LD;
	// Dual DAC 8-bit right aligned data register
	__IO uint32_t DHR8RD;
	// Channel 1 data output register
	__I uint32_t DOR1;
	// Channel 2 data output register
	__I uint32_t DOR2;
} DAC_TypeDef;

// DMA channel register access structure
typedef struct {
	__IO uint32_t CCR;
	__IO uint32_t CNDTR;
	__IO uint32_t CPAR;
	__IO uint32_t CMAR;
} DMA_Channel_TypeDef;

// DMA system control register access structure
typedef struct {
	__IO uint32_t ISR;
	__IO uint32_t IFCR;
} DMA_TypeDef;

// External interrupt register access structure
typedef struct {
	// Interrupt mask register
	__IO uint32_t IMR;
	// Event mask register
	__IO uint32_t EMR;
	// Rising trigger selection register
	__IO uint32_t RTSR;
	// Falling trigger selection register
	__IO uint32_t FTSR;
	// Software interrupt event register
	__IO uint32_t SWIER;
	// Interrupt pending register
	__IO uint32_t PR;
} EXTI_TypeDef;

// FLASH memory register access structure
typedef struct {
	__IO uint32_t ACR;
	// Key register
	__IO uint32_t KEYR;
	// Optional key register
	__IO uint32_t OPTKEYR;
	// Status register
	__IO uint32_t SR;
	// Configuration register
	__IO uint32_t CR;
	__IO uint32_t AR;
	__IO uint32_t RESERVED;
	__IO uint32_t OBR;
	// Write protection register
	__IO uint32_t WRPR;
} FLASH_TypeDef;

// GPIO register access structure
typedef struct {
	// Configuration register low
	__IO uint32_t CRL;
	// Configuration register high
	__IO uint32_t CRH;
	// Input data register
	__IO uint32_t IDR;
	// Output data register
	__IO uint32_t ODR;
	// Bit set register
	__IO uint32_t BSRR;
	// Bit reset register
	__IO uint32_t BRR;
	// Configuration lock register
	__IO uint32_t LCKR;
} GPIO_TypeDef;

// Alternate I/O pin mapping register access structure
typedef struct {
	// Event control register
	__IO uint32_t EVCR;
	// Alternate function remap register #1
	__IO uint32_t MAPR;
	// External interrupt configuration registers
	__IO uint32_t EXTICR[4];
	uint32_t RESERVED0;
	// Alternate function remap register #2
	__IO uint32_t MAPR2;
} AFIO_TypeDef;

// Interrupt controller register access structure
typedef struct {
	// Interrupt Set Enable register
	__IO uint32_t ISER[8];
	uint32_t RESERVED0[24];
	// Interrupt Clear Enable register
	__IO uint32_t ICER[8];
	uint32_t RESERVED1[24];
	// Interrupt Set Pending register
	__IO uint32_t ISPR[8];
	uint32_t RESERVED2[24];
	// Interrupt Clear Pending register
	__IO uint32_t ICPR[8];
	uint32_t RESERVED3[24];
	// Interrupt Routine Active register
	__I uint32_t IABR[8];
	uint32_t RESERVED4[56];
	// Interrupt Priority register
	__IO uint8_t IP[240];
	uint32_t RESERVED5[644];
	// Software Trigger Interrupt register
	__IO uint32_t STIR;
} NVIC_TypeDef;

// Reset and clock control register access structure
typedef struct {
	__IO uint32_t CR;
	__IO uint32_t CFGR;
	__IO uint32_t CIR;
	// APB2 peripheral reset register
	__IO uint32_t APB2RSTR;
	// APB1 peripheral reset register
	__IO uint32_t APB1RSTR;
	// AHB clock enable register
	__IO uint32_t AHBENR;
	// APB2 clock enable register
	__IO uint32_t APB2ENR;
	// APB1 clock enable register
	__IO uint32_t APB1ENR;
	__IO uint32_t BDCR;
	__IO uint32_t CSR;
} RCC_TypeDef;

// I2C register access structure
typedef struct {
	// Configuration register #1
	__IO uint16_t CR1;
	uint16_t RESERVED0;
	// Configuration register #2
	__IO uint16_t CR2;
	uint16_t RESERVED1;
	// Own-address register #1
	__IO uint16_t OAR1;
	uint16_t RESERVED2;
	// Own-address register #2
	__IO uint16_t OAR2;
	uint16_t RESERVED3;
	// Data register
	__IO uint16_t DR;
	uint16_t RESERVED4;
	// Status register #1
	__IO uint16_t SR1;
	uint16_t RESERVED5;
	// Status register #2
	__IO uint16_t SR2;
	uint16_t RESERVED6;
	// Clock control register
	__IO uint16_t CCR;
	uint16_t RESERVED7;
	// Tristate enable register
	__IO uint16_t TRISE;
	uint16_t RESERVED8;
} I2C_TypeDef;

// Independent watchdog register access structure
typedef struct {
	// Key register
	__IO uint32_t KR;
	// Prescaler register
	__IO uint32_t PR;
	// Reload register
	__IO uint32_t RLR;
	// Status register
	__IO uint32_t SR;
} IWDG_TypeDef;

// System Control Block register access structure
typedef struct {
	// CPU ID register
	__I uint32_t CPUID;
	// Interrupt Control State register
	__IO uint32_t ICSR;
	// Vector Table Offset register
	__IO uint32_t VTOR;
	// Application Interrupt and Reset Control register
	__IO uint32_t AIRCR;
	// System Control register
	__IO uint32_t SCR;
	// Configuration Control register
	__IO uint32_t CCR;
	// System Handler Priority register
	__IO uint8_t SHP[12];
	// System Handler Control and State register
	__IO uint32_t SHCSR;
	// Configurable Fault status register
	__IO uint32_t CFSR;
	// Hard fault status register
	__IO uint32_t HFSR;
	// Debug fault status register
	__IO uint32_t DFSR;
	// Memory management address register
	__IO uint32_t MMFAR;
	// Bus fault address register
	__IO uint32_t BFAR;
	// Auxiliary fault address register
	__IO uint32_t AFSR;
	// Processor Feature register
	__I uint32_t PFR[2];
	// Debug Feature register
	__I uint32_t DFR;
	// Auxiliary Feature register
	__I uint32_t ADR;
	// Memory Model Feature register
	__I uint32_t MMFR[4];
	// ISA Feature register
	__I uint32_t ISAR[5];
} SCB_TypeDef;

// SPI register access structure
typedef struct {
	// Configuration register #1
	__IO uint16_t CR1;
	uint16_t RESERVED0;
	// Configuration register #2
	__IO uint16_t CR2;
	uint16_t RESERVED1;
	// Status register
	__IO uint16_t SR;
	uint16_t RESERVED2;
	// Data register
	__IO uint16_t DR;
	uint16_t RESERVED3;
	// CRC polynomial register
	__IO uint16_t CRCPR;
	uint16_t RESERVED4;
	// Receive CRC register
	__IO uint16_t RXCRCR;
	uint16_t RESERVED5;
	// Transmit CRC register
	__IO uint16_t TXCRCR;
	uint16_t RESERVED6;
	// I2S configuration register
	__IO uint16_t I2SCFGR;
	uint16_t RESERVED7;
	// I2S prescaler register
	__IO uint16_t I2SPR;
	uint16_t RESERVED8;
} SPI_TypeDef;

// SysTick register access structure
typedef struct {
	// Control and status register
	__IO uint32_t CTRL;
	// Reload value register
	__IO uint32_t LOAD;
	// Current value register
	__IO uint32_t VAL;
	// SysTick Calibration register
	__I uint32_t CALIB;
} SysTick_TypeDef;

// Timer control register access structure
typedef struct {
	// Configuration register #1
	__IO uint16_t CR1;
	uint16_t RESERVED0;
	// Configuration register #2
	__IO uint16_t CR2;
	uint16_t RESERVED1;
	// Slave mode configuration register
	__IO uint16_t SMCR;
	uint16_t RESERVED2;
	// Interrupt enable register
	__IO uint16_t DIER;
	uint16_t RESERVED3;
	// Status register
	__IO uint16_t SR;
	uint16_t RESERVED4;
	// Event generation register
	__IO uint16_t EGR;
	uint16_t RESERVED5;
	// Capture/compare mode register #1
	__IO uint16_t CCMR1;
	uint16_t RESERVED6;
	// Capture/compare mode register #2
	__IO uint16_t CCMR2;
	uint16_t RESERVED7;
	// Capture/compare error register
	__IO uint16_t CCER;
	uint16_t RESERVED8;
	// Timer count register
	__IO uint16_t CNT;
	uint16_t RESERVED9;
	// Timer prescaler register
	__IO uint16_t PSC;
	uint16_t RESERVED10;
	// Auto-reload register
	__IO uint16_t ARR;
	uint16_t RESERVED11;
	// Repetition counter register
	__IO uint16_t RCR;
	uint16_t RESERVED12;
	// Capture/compare value register #1
	__IO uint16_t CCR1;
	uint16_t RESERVED13;
	// Capture/compare value register #2
	__IO uint16_t CCR2;
	uint16_t RESERVED14;
	// Capture/compare value register #3
	__IO uint16_t CCR3;
	uint16_t RESERVED15;
	// Capture/compare value register #4
	__IO uint16_t CCR4;
	uint16_t RESERVED16;
	// Break and dead time register
	__IO uint16_t BDTR;
	uint16_t RESERVED17;
	// DMA control register
	__IO uint16_t DCR;
	uint16_t RESERVED18;
	// DMA address for full transfer
	__IO uint16_t DMAR;
	uint16_t RESERVED19;
} TIM_TypeDef;

// USART control register access structure
typedef struct {
	// Status register
	__IO uint16_t SR;
	uint16_t RESERVED0;
	// Data register
	__IO uint16_t DR;
	uint16_t RESERVED1;
	// Baud rate register
	__IO uint16_t BRR;
	uint16_t RESERVED2;
	// Configuration register #1
	__IO uint16_t CR1;
	uint16_t RESERVED3;
	// Configuration register #2
	__IO uint16_t CR2;
	uint16_t RESERVED4;
	// Configuration register #3
	__IO uint16_t CR3;
	uint16_t RESERVED5;
	// Guard time and prescaler register
	__IO uint16_t GTPR;
	uint16_t RESERVED6;
} USART_TypeDef;

// USB register access structure
#define USB_NR_EP_REGS 8
typedef struct {
	// Endpoint registers
	__IO uint32_t EP[USB_NR_EP_REGS];
	uint32_t RESERVED[8];
	// Control register
	__IO uint32_t CNTR;
	// Interrupt status register
	__IO uint32_t ISTR;
	// Frame number register
	__IO uint32_t FNR;
	// Device address register
	__IO uint32_t DADDR;
	// Buffer table address register, 8-byte aligned
	__IO uint32_t BTABLE;
	/*
	 * Address offset within the USB
	 * packet memory area which points
	 * to the base of the buffer
	 * descriptor table.  Must be
	 * aligned to an 8 byte boundary.
	 */
} USB_TypeDef;

// Window watchdog register access structure
typedef struct {
	// Control register
	__IO uint32_t CR;
	// Configuration register
	__IO uint32_t CFR;
	// Status register
	__IO uint32_t SR;
} WWDG_TypeDef;

// Peripheral memory map: addresses in the direct region
// Flash base address
#define FLASH_BASE ((uint32_t)0x08005000)
// RAM base address
#define SRAM_BASE ((uint32_t)0x20000000)
// Peripheral base address
#define PERIPH_BASE ((uint32_t)0x40000000)
// SCS base address
#define SCS_BASE ((uint32_t)0xE000E000)

// APB1 peripherals base address
#define APB1PERIPH_BASE PERIPH_BASE
// APB2 peripherals base address
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000)
// AHB peripherals base address
#define AHBPERIPH_BASE (PERIPH_BASE + 0x20000)

// Peripherals on APB1
// Timer #2
#define TIM2_BASE (APB1PERIPH_BASE + 0x0000)
#define TIM2 ((TIM_TypeDef*)TIM2_BASE)
// Timer #3
#define TIM3_BASE (APB1PERIPH_BASE + 0x0400)
#define TIM3 ((TIM_TypeDef*)TIM3_BASE)
// Timer #4
#define TIM4_BASE (APB1PERIPH_BASE + 0x0800)
#define TIM4 ((TIM_TypeDef*)TIM4_BASE)
#ifdef MAPLE_RET6
// Timer #5
#define TIM5_BASE (APB1PERIPH_BASE + 0x0C00)
#define TIM5 ((TIM_TypeDef*)TIM5_BASE)
// Timer #6
#define TIM6_BASE (APB1PERIPH_BASE + 0x1000)
#define TIM6 ((TIM_TypeDef*)TIM6_BASE)
// Timer #7
#define TIM7_BASE (APB1PERIPH_BASE + 0x1400)
#define TIM7 ((TIM_TypeDef*)TIM7_BASE)
#endif
// Window watchdog
#define WWDG_BASE (APB1PERIPH_BASE + 0x2C00)
#define WWDG ((WWDG_TypeDef*)WWDG_BASE)
// Independent watchdog
#define IWDG_BASE (APB1PERIPH_BASE + 0x3000)
#define IWDG ((IWDG_TypeDef*)IWDG_BASE)
// SPI #2
#define SPI2_BASE (APB1PERIPH_BASE + 0x3800)
#define SPI2 ((SPI_TypeDef*)SPI2_BASE)
#ifdef MAPLE_RET6
// SPI #3
#define SPI3_BASE (APB1PERIPH_BASE + 0x3C00)
#define SPI3 ((SPI_TypeDef*)SPI3_BASE)
#endif
// USART #2
#define USART2_BASE (APB1PERIPH_BASE + 0x4400)
#define USART2 ((USART_TypeDef*)USART2_BASE)
// USART #3
#define USART3_BASE (APB1PERIPH_BASE + 0x4800)
#define USART3 ((USART_TypeDef*)USART3_BASE)
#ifdef MAPLE_RET6
// UART #4
#define UART4_BASE (APB1PERIPH_BASE + 0x4C00)
#define UART4 ((USART_TypeDef*)UART4_BASE)
// UART #5
#define UART5_BASE (APB1PERIPH_BASE + 0x5000)
#define UART5 ((USART_TypeDef*)UART5_BASE)
#endif
// I2C #1
#define I2C1_BASE (APB1PERIPH_BASE + 0x5400)
#define I2C1 ((I2C_TypeDef*)I2C1_BASE)
// I2C #2
#define I2C2_BASE (APB1PERIPH_BASE + 0x5800)
#define I2C2 ((I2C_TypeDef*)I2C2_BASE)
// USB
#define USB_BASE (APB1PERIPH_BASE + 0x5C00)
#define USB ((USB_TypeDef*)USB_BASE)
// PMA (shared between USB and CAN) peripheral shared RAM
#define PMA_BASE (APB1PERIPH_BASE + 0x6000)
#define PMA ((__IO uint8_t*)PMA_BASE)
// DAC
#define DAC_BASE (APB1PERIPH_BASE + 0x7400)
#define DAC ((DAC_TypeDef*)DAC_BASE)

// Peripherals on APB2
// Alternate function I/O
#define AFIO_BASE (APB2PERIPH_BASE + 0x0000)
#define AFIO ((AFIO_TypeDef*)AFIO_BASE)
// External interrupt management
#define EXTI_BASE (APB2PERIPH_BASE + 0x0400)
#define EXTI ((EXTI_TypeDef*)EXTI_BASE)
// GPIO port A
#define GPIOA_BASE (APB2PERIPH_BASE + 0x0800)
#define GPIOA ((GPIO_TypeDef*)GPIOA_BASE)
// GPIO port B
#define GPIOB_BASE (APB2PERIPH_BASE + 0x0C00)
#define GPIOB ((GPIO_TypeDef*)GPIOB_BASE)
// GPIO port C
#define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)
#define GPIOC ((GPIO_TypeDef*)GPIOC_BASE)
// GPIO port D
#define GPIOD_BASE (APB2PERIPH_BASE + 0x1400)
#define GPIOD ((GPIO_TypeDef*)GPIOD_BASE)
// ADC #1
#define ADC1_BASE (APB2PERIPH_BASE + 0x2400)
#define ADC1 ((ADC_TypeDef*)ADC1_BASE)
// ADC #2
#define ADC2_BASE (APB2PERIPH_BASE + 0x2800)
#define ADC2 ((ADC_TypeDef*)ADC2_BASE)
// Timer #1
#define TIM1_BASE (APB2PERIPH_BASE + 0x2C00)
#define TIM1 ((TIM_TypeDef*)TIM1_BASE)
// SPI #1
#define SPI1_BASE (APB2PERIPH_BASE + 0x3000)
#define SPI1 ((SPI_TypeDef*)SPI1_BASE)
#ifdef MAPLE_RET6
// Timer #8
#define TIM8_BASE (APB2PERIPH_BASE + 0x3400)
#define TIM8 ((TIM_TypeDef*)TIM8_BASE)
#endif
// USART #1
#define USART1_BASE (APB2PERIPH_BASE + 0x3800)
#define USART1 ((USART_TypeDef*)USART1_BASE)
#ifdef MAPLE_RET6
// ADC #3
#define ADC3_BASE (APB2PERIPH_BASE + 0x3C00)
#define ADC3 ((ADC_TypeDef*)ADC3_BASE)
#endif

// Reset and clock control
#define RCC_BASE (AHBPERIPH_BASE + 0x1000)
#define RCC ((RCC_TypeDef*)RCC_BASE)
// DMA #1
#define DMA1_BASE (AHBPERIPH_BASE + 0x0000)
#define DMA1 ((DMA_TypeDef*)DMA1_BASE)
#define DMA1_Channel1_BASE (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel1 ((DMA_Channel_TypeDef*)DMA1_Channel1_BASE)
#define DMA1_Channel2_BASE (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel2 ((DMA_Channel_TypeDef*)DMA1_Channel2_BASE)
#define DMA1_Channel3_BASE (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel3 ((DMA_Channel_TypeDef*)DMA1_Channel3_BASE)
#define DMA1_Channel4_BASE (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel4 ((DMA_Channel_TypeDef*)DMA1_Channel4_BASE)
#define DMA1_Channel5_BASE (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel5 ((DMA_Channel_TypeDef*)DMA1_Channel5_BASE)
#define DMA1_Channel6_BASE (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel6 ((DMA_Channel_TypeDef*)DMA1_Channel6_BASE)
#define DMA1_Channel7_BASE (AHBPERIPH_BASE + 0x0080)
#define DMA1_Channel7 ((DMA_Channel_TypeDef*)DMA1_Channel7_BASE)
// DMA #2
#define DMA2_BASE (AHBPERIPH_BASE + 0x0400)
#define DMA2 ((DMA_TypeDef*)DMA2_BASE)
#define DMA2_Channel1_BASE (AHBPERIPH_BASE + 0x0408)
#define DMA2_Channel1 ((DMA_Channel_TypeDef*)DMA2_Channel1_BASE)
#define DMA2_Channel2_BASE (AHBPERIPH_BASE + 0x041C)
#define DMA2_Channel2 ((DMA_Channel_TypeDef*)DMA2_Channel2_BASE)
#define DMA2_Channel3_BASE (AHBPERIPH_BASE + 0x0430)
#define DMA2_Channel3 ((DMA_Channel_TypeDef*)DMA2_Channel3_BASE)
#define DMA2_Channel4_BASE (AHBPERIPH_BASE + 0x0444)
#define DMA2_Channel4 ((DMA_Channel_TypeDef*)DMA2_Channel4_BASE)
#define DMA2_Channel5_BASE (AHBPERIPH_BASE + 0x0458)
#define DMA2_Channel5 ((DMA_Channel_TypeDef*)DMA2_Channel5_BASE)
// SysTick
#define SysTick_BASE (SCS_BASE + 0x0010)
#define SysTick ((SysTick_TypeDef*)SysTick_BASE)
// NVIC
#define NVIC_BASE (SCS_BASE + 0x0100)
#define NVIC ((NVIC_TypeDef*)NVIC_BASE)
// System Control Block
#define SCB_BASE (SCS_BASE + 0x0D00)
#define SCB ((SCB_TypeDef*)SCB_BASE)
// Flash memory
#define FLASH_R_BASE (AHBPERIPH_BASE + 0x2000)
#define FLASH ((FLASH_TypeDef*)FLASH_R_BASE)

// ADC defines
// ADC scan mode enable
#define ADC_CR1_SCAN ((uint32_t)0x00000100)

// ADC on/off flag
#define ADC_CR2_ADON ((uint32_t)0x00000001)
// ADC continuous conversion enable/disable
#define ADC_CR2_CONT ((uint32_t)0x00000002)
// ADC calibration in progress?
#define ADC_CR2_CAL ((uint32_t)0x00000004)
// Reset ADC calibration
#define ADC_CR2_RSTCAL ((uint32_t)0x00000008)
// Enable ADC DMA requests
#define ADC_CR2_DMA ((uint32_t)0x00000100)
// Disables ADC external trigger
#define ADC_CR2_NOTRIG ((uint32_t)0x000E0000)
// Starts software conversion loop
#define ADC_CR2_SWSTART ((uint32_t)0x00500000)
// Temperature sensor and reference voltage enable
#define ADC_CR2_TSVREFE ((uint32_t)0x00800000)

// Mapping defines
// Remap I2C1 to PB8..PB9
#define AFIO_MAPR_I2C1_REMAP ((uint32_t)0x00000002)
// Remap USART2 to PD3..PD7
#define AFIO_MAPR_USART2_REMAP ((uint32_t)0x00000008)
// Remap USART3 to PC10..PC12 and PB13..14
#define AFIO_MAPR_USART3_REMAP ((uint32_t)0x00000010)
// Remap TIM4 to PD12..15
#define AFIO_MAPR_TIM4_REMAP ((uint32_t)0x00001000)

// DAC defines
// DAC channel 1 enable/disable flag
#define DAC_CR_EN1 ((uint32_t)0x00000001)

// DMA defines
// DMA channel enable
#define DMA_CCR_EN ((uint32_t)0x00000001)
// Peripheral to memory
#define DMA_CCR_SRC ((uint32_t)0x00000000)
// Memory to peripheral
#define DMA_CCR_DST ((uint32_t)0x00000010)
// Circular mode enable
#define DMA_CCR_CIRC ((uint32_t)0x00000020)
// Auto-increment peripheral address
#define DMA_CCR_PERIPHINC ((uint32_t)0x00000040)
// Auto-increment memory address
#define DMA_CCR_MEMINC ((uint32_t)0x00000080)
// Source size of byte
#define DMA_CCR_SRC_BYTE ((uint32_t)0x00000000)
// Source size of halfword
#define DMA_CCR_SRC_HWORD ((uint32_t)0x00000100)
// Source size of word
#define DMA_CCR_SRC_WORD ((uint32_t)0x00000200)
// Destination size of byte
#define DMA_CCR_DST_BYTE ((uint32_t)0x00000000)
// Destination size of halfword
#define DMA_CCR_DST_HWORD ((uint32_t)0x00000400)
// Destination size of word
#define DMA_CCR_DST_WORD ((uint32_t)0x00000800)
// Low priority transfer
#define DMA_CCR_PRI_LOW ((uint32_t)0x00000000)
// Medium priority transfer
#define DMA_CCR_PRI_MED ((uint32_t)0x00001000)
// High priority transfer
#define DMA_CCR_PRI_HIGH ((uint32_t)0x00002000)
// Very high priority transfer
#define DMA_CCR_PRI_MAX ((uint32_t)0x00003000)
// Memory-to-memory?
#define DMA_CCR_M2M ((uint32_t)0x00004000)

// Flash memory defines
// Bits for Flash latency cycles
#define FLASH_ACR_LATENCY ((uint8_t)0x03)
// Two wait states for Flash access
#define FLASH_ACR_LATENCY_2 ((uint8_t)0x02)
// Prefetch buffer enable
#define FLASH_ACR_PRFTBE ((uint8_t)0x10)

// I2C defines
// I2C enable
#define I2C_CR1_PE ((uint16_t)0x0001)
// Start-bit generation flag
#define I2C_CR1_START ((uint16_t)0x0100)
// Stop-bit generation flag
#define I2C_CR1_STOP ((uint16_t)0x0200)
// Acknowledge enable flag
#define I2C_CR1_ACK ((uint16_t)0x0400)
// Acknowledge position
#define I2C_CR1_POS ((uint16_t)0x0800)
// Software reset flag
#define I2C_CR1_SWRST ((uint16_t)0x8000)

// I2C error interrupt enable
#define I2C_CR2_ITERREN ((uint16_t)0x0100)
// I2C event interrupt enable
#define I2C_CR2_ITEVTEN ((uint16_t)0x0200)
// I2C buffer interrupt enable
#define I2C_CR2_ITBUFEN ((uint16_t)0x0400)

// Addressing mode flag
#define I2C_OAR1_ADDMODE ((uint16_t)0x8000)
// Dual addressing mode enable
#define I2C_OAR2_ENDUAL ((uint8_t)0x01)

// Start bit flag
#define I2C_SR1_SB ((uint16_t)0x0001)
// Address-sent flag
#define I2C_SR1_ADDR ((uint16_t)0x0002)
// Transfer finished flag
#define I2C_SR1_BTF ((uint16_t)0x0004)
// Receiver data full flag
#define I2C_SR1_RXNE ((uint16_t)0x0040)
// Transmitter data empty flag
#define I2C_SR1_TXE ((uint16_t)0x0080)
// Bus error flag
#define I2C_SR1_BERR ((uint16_t)0x0100)
// Arbitration lost flag
#define I2C_SR1_ARLO ((uint16_t)0x0200)
// Acknowledge failure flag
#define I2C_SR1_AF ((uint16_t)0x0400)
// Timeout flag
#define I2C_SR1_OVR ((uint16_t)0x0800)

// Master/slave flag
#define I2C_SR2_MSL ((uint16_t)0x0001)
// Bus busy flag
#define I2C_SR2_BUSY ((uint16_t)0x0002)
// Transmitter/receiver flag
#define I2C_SR2_TRA ((uint16_t)0x0004)

// Fast mode selection
#define I2C_CCR_FS ((uint16_t)0x8000)

// RCC defines
// Enable HSE
#define RCC_CR_HSEON ((uint32_t)0x00010000)
// HSE ready?
#define RCC_CR_HSERDY ((uint32_t)0x00020000)
// Bypass HSE for external oscillator
#define RCC_CR_HSEBYP ((uint32_t)0x00040000)
// Enable PLL
#define RCC_CR_PLLON ((uint32_t)0x01000000)
// PLL ready?
#define RCC_CR_PLLRDY ((uint32_t)0x02000000)

// Bits for system clock select
#define RCC_CFGR_SW ((uint32_t)0x00000003)
// Set PLL as system clock
#define RCC_CFGR_SW_PLL ((uint32_t)0x00000002)
// Bits for system clock status
#define RCC_CFGR_SWS ((uint32_t)0x0000000C)
// PLL is used as system clock
#define RCC_CFGR_SWS_PLL ((uint32_t)0x00000008)
// Bits for AHB clock divider (HCLK)
#define RCC_CFGR_HPRE ((uint32_t)0x000000F0)
// Bits for APB1 clock divider
#define RCC_CFGR_PPRE1 ((uint32_t)0x00000700)
// Bits for APB2 clock divider
#define RCC_CFGR_PPRE2 ((uint32_t)0x00003800)
// Bits for ADC clock divider
#define RCC_CFGR_ADCPRE ((uint32_t)0x0000C000)
// Bits for PLL multiplier
#define RCC_CFGR_PLLMUL ((uint32_t)0x003F0000)
// Divide APB1 clock by 2
#define RCC_CFGR_PPRE1_DIV2 ((uint32_t)0x00000400)
// Divide ADC clock by 6
#define RCC_CFGR_ADCPRE_DIV6 ((uint32_t)0x00008000)
// Multiply PLL by 9
#define RCC_CFGR_PLLMUL_9 ((uint32_t)0x001C0000)
// PLL source from HSE
#define RCC_CFGR_PLLSRC_HSE ((uint32_t)0x00010000)

// Reset alternate I/O functions on APB2
#define RCC_APB2RSTR_AFIORST ((uint32_t)0x00000001)
// Reset I/O port A on APB2
#define RCC_APB2RSTR_IOPARST ((uint32_t)0x00000004)
// Reset I/O port B on APB2
#define RCC_APB2RSTR_IOPBRST ((uint32_t)0x00000008)
// Reset I/O port C on APB2
#define RCC_APB2RSTR_IOPCRST ((uint32_t)0x00000010)
// Reset I/O port D on APB2
#define RCC_APB2RSTR_IOPDRST ((uint32_t)0x00000020)
// Reset ADC1 on APB2
#define RCC_APB2RSTR_ADC1RST ((uint32_t)0x00000200)
// Reset ADC2 on APB2
#define RCC_APB2RSTR_ADC2RST ((uint32_t)0x00000400)
// Reset TIM1 on APB2
#define RCC_APB2RSTR_TIM1RST ((uint32_t)0x00000800)
// Reset SPI1 on APB2
#define RCC_APB2RSTR_SPI1RST ((uint32_t)0x00001000)
#ifdef MAPLE_RET6
// Reset TIM8 on APB2
#define RCC_APB2RSTR_TIM8RST ((uint32_t)0x00002000)
#endif
// Reset USART1 on APB2
#define RCC_APB2RSTR_USART1RST ((uint32_t)0x00004000)
// Reset ADC3 on APB2
#define RCC_APB2RSTR_ADC3RST ((uint32_t)0x00008000)

// Reset TIM2 on APB1
#define RCC_APB1RSTR_TIM2RST ((uint32_t)0x00000001)
// Reset TIM3 on APB1
#define RCC_APB1RSTR_TIM3RST ((uint32_t)0x00000002)
// Reset USART2 on APB1
#define RCC_APB1RSTR_USART2RST ((uint32_t)0x00020000)
// Reset I2C1 on APB1
#define RCC_APB1RSTR_I2C1RST ((uint32_t)0x00200000)
// Reset TIM4 on APB1
#define RCC_APB1RSTR_TIM4RST ((uint32_t)0x00000004)
// Reset SPI2 on APB1
#define RCC_APB1RSTR_SPI2RST ((uint32_t)0x00004000)
// Reset USART3 on APB1
#define RCC_APB1RSTR_USART3RST ((uint32_t)0x00040000)
// Reset I2C2 on APB1
#define RCC_APB1RSTR_I2C2RST ((uint32_t)0x00400000)
#ifdef MAPLE_RET6
// Reset TIM5 on APB1
#define RCC_APB1RSTR_TIM5RST ((uint32_t)0x00000008)
// Reset TIM6 on APB1
#define RCC_APB1RSTR_TIM6RST ((uint32_t)0x00000010)
// Reset TIM7 on APB1
#define RCC_APB1RSTR_TIM7RST ((uint32_t)0x00000020)
// Reset SPI3 on APB1
#define RCC_APB1RSTR_SPI3RST ((uint32_t)0x00008000)
// Reset UART4 on APB1
#define RCC_APB1RSTR_UART4RST ((uint32_t)0x00080000)
// Reset UART5 on APB1
#define RCC_APB1RSTR_UART5RST ((uint32_t)0x00100000)
#endif
// Reset USB on APB1
#define RCC_APB1RSTR_USBRST ((uint32_t)0x00800000)
// Reset DAC on APB1
#define RCC_APB1RSTR_DACRST ((uint32_t)0x20000000)

// Enable AFIO clock
#define RCC_APB2ENR_AFIOEN ((uint32_t)0x00000001)
// Enable I/O port A clock
#define RCC_APB2ENR_IOPAEN ((uint32_t)0x00000004)
// Enable I/O port B clock
#define RCC_APB2ENR_IOPBEN ((uint32_t)0x00000008)
// Enable I/O port C clock
#define RCC_APB2ENR_IOPCEN ((uint32_t)0x00000010)
// Enable I/O port D clock
#define RCC_APB2ENR_IOPDEN ((uint32_t)0x00000020)
// Enable ADC1 clock
#define RCC_APB2ENR_ADC1EN ((uint32_t)0x00000200)
// Enable ADC2 clock
#define RCC_APB2ENR_ADC2EN ((uint32_t)0x00000400)
// Enable TIM1 clock
#define RCC_APB2ENR_TIM1EN ((uint32_t)0x00000800)
// Enable SPI1 clock
#define RCC_APB2ENR_SPI1EN ((uint32_t)0x00001000)
// Enable USART1 clock
#define RCC_APB2ENR_USART1EN ((uint32_t)0x00004000)
#ifdef MAPLE_RET6
// Enable TIM8 clock
#define RCC_APB2ENR_TIM8EN ((uint32_t)0x00002000)
#endif

// Enable TIM2 clock
#define RCC_APB1ENR_TIM2EN ((uint32_t)0x00000001)
// Enable TIM3 clock
#define RCC_APB1ENR_TIM3EN ((uint32_t)0x00000002)
// Enable USART2 clock
#define RCC_APB1ENR_USART2EN ((uint32_t)0x00020000)
// Enable I2C1 clock
#define RCC_APB1ENR_I2C1EN ((uint32_t)0x00200000)
// Enable TIM4 clock
#define RCC_APB1ENR_TIM4EN ((uint32_t)0x00000004)
// Enable SPI2 clock
#define RCC_APB1ENR_SPI2EN ((uint32_t)0x00004000)
// Enable USART3 clock
#define RCC_APB1ENR_USART3EN ((uint32_t)0x00040000)
// Enable I2C2 clock
#define RCC_APB1ENR_I2C2EN ((uint32_t)0x00400000)
#ifdef MAPLE_RET6
// Enable TIM5 clock
#define RCC_APB1ENR_TIM5EN ((uint32_t)0x00000008)
// Enable TIM6 clock
#define RCC_APB1ENR_TIM6EN ((uint32_t)0x00000010)
// Enable TIM7 clock
#define RCC_APB1ENR_TIM7EN ((uint32_t)0x00000020)
// Enable SPI3 clock
#define RCC_APB1ENR_SPI3EN ((uint32_t)0x00008000)
// Enable UART4 clock
#define RCC_APB1ENR_UART4EN ((uint32_t)0x00080000)
// Enable UART5 clock
#define RCC_APB1ENR_UART5EN ((uint32_t)0x00100000)
#endif
// Enable USB clock
#define RCC_APB1ENR_USBEN ((uint32_t)0x00800000)
// Enable DAC clock
#define RCC_APB1ENR_DACEN ((uint32_t)0x20000000)

// Enable SDIO clock
#define RCC_AHBENR_SDIOEN ((uint32_t)0x00000400)
// Enable FSMC clock
#define RCC_AHBENR_FSMCEN ((uint32_t)0x00000100)
// Enable CRC clock
#define RCC_AHBENR_CRCEN ((uint32_t)0x00000040)
// Enable FLITF clock
#define RCC_AHBENR_FLITFEN ((uint32_t)0x00000010)
// Enable SRAM clock
#define RCC_AHBENR_SRAMEN ((uint32_t)0x00000004)
// Enable DMA2 clock
#define RCC_AHBENR_DMA2EN ((uint32_t)0x00000002)
// Enable DMA1 clock
#define RCC_AHBENR_DMA1EN ((uint32_t)0x00000001)

// SCB defines
// Request reset
#define SCB_AIRCR_RESET ((uint32_t)0x00000004)
// Priority group #3 (4 bits for preemption priority, no bits for subpriority)
#define SCB_AIRCR_PRIGROUP_3 ((uint32_t)0x00000300)
// Priority group mask
#define SCB_AIRCR_PRIGROUP ((uint32_t)0x00000700)
// Key to allow mutation of application interrupt register
#define SCB_AIRCR_VECTKEY ((uint32_t)0x05FA0000)

// PendSV set
#define SCB_ICSR_PENDSV ((uint32_t)0x10000000)

// Enable bus fault handler
#define SCB_SHCSR_BUSFAULTENA ((uint32_t)0x00020000)
// Enable illegal instruction fault handler
#define SCB_SHCSR_USGFAULTENA ((uint32_t)0x00040000)

// SPI defines
// CPHA = 1 (even edges)
#define SPI_CR1_CPHA_1 ((uint16_t)0x0001)
// CPOL = 1 (active low clock)
#define SPI_CR1_CPOL_1 ((uint16_t)0x0002)
// Baud rate PCLK/2
#define SPI_CR1_DIV2 ((uint16_t)0x0000)
// Baud rate PCLK/4
#define SPI_CR1_DIV4 ((uint16_t)0x0008)
// Baud rate PCLK/8
#define SPI_CR1_DIV8 ((uint16_t)0x0010)
// Baud rate PCLK/16
#define SPI_CR1_DIV16 ((uint16_t)0x0018)
// Baud rate PCLK/32
#define SPI_CR1_DIV32 ((uint16_t)0x0020)
// Baud rate PCLK/64
#define SPI_CR1_DIV64 ((uint16_t)0x0028)
// Baud rate PCLK/128
#define SPI_CR1_DIV128 ((uint16_t)0x0030)
// Baud rate PCLK/256
#define SPI_CR1_DIV256 ((uint16_t)0x0038)
// Least significant bit first
#define SPI_CR1_LSBFIRST ((uint16_t)0x0080)
// SPI enable
#define SPI_CR1_SPE ((uint16_t)0x0040)
// SPI master mode
#define SPI_CR1_MSTR ((uint16_t)0x0104)
// Software slave selection
#define SPI_CR1_NSS_SOFT ((uint16_t)0x0200)
// 16-bit data size
#define SPI_CR1_16BIT ((uint16_t)0x0800)

// Receive buffer full interrupt enable
#define SPI_CR2_RXNEIE ((uint16_t)0x0040)
// Transmit buffer empty interrupt enable
#define SPI_CR2_TXEIE ((uint16_t)0x0080)

// Transmit data empty flag
#define SPI_SR_TXE ((uint16_t)0x0002)
// Receive buffer full flag
#define SPI_SR_RXNE ((uint16_t)0x0001)

// SysTick defines
// SysTick enable
#define SysTick_CTRL_ENABLE ((uint32_t)0x00000001)
// SysTick interrupt enable
#define SysTick_CTRL_TICKINT ((uint32_t)0x00000002)

// Timer defines
// Auto reload preload enable
#define TIM_CR1_ARPE ((uint16_t)0x0080)
// Count enable [run timer]
#define TIM_CR1_CEN ((uint16_t)0x0001)

// Master mode selection to trigger on update [act as prescaler]
#define TIM_CR2_MMS_UPDATE ((uint16_t)0x0020)

// Slave mode selection to count on trigger
#define TIM_SMCR_SMS_EXTERNAL ((uint16_t)0x0007)
// Select internal trigger #0
#define TIM_SMCR_TS_ITR0 ((uint16_t)0x0000)
// Select internal trigger #1
#define TIM_SMCR_TS_ITR1 ((uint16_t)0x0010)
// Select internal trigger #2
#define TIM_SMCR_TS_ITR2 ((uint16_t)0x0020)
// Select internal trigger #3
#define TIM_SMCR_TS_ITR3 ((uint16_t)0x0030)

// Update interrupt enable
#define TIM_DIER_UIE ((uint16_t)0x0001)
// CC1 interrupt enable
#define TIM_DIER_CC1IE ((uint16_t)0x0002)
// CC2 interrupt enable
#define TIM_DIER_CC2IE ((uint16_t)0x0004)
// CC3 interrupt enable
#define TIM_DIER_CC3IE ((uint16_t)0x0008)
// CC4 interrupt enable
#define TIM_DIER_CC4IE ((uint16_t)0x0010)

// Update interrupt pending
#define TIM_SR_UIF ((uint16_t)0x0001)
// CC1 interrupt pending
#define TIM_SR_CC1IF ((uint16_t)0x0002)
// CC2 interrupt pending
#define TIM_SR_CC2IF ((uint16_t)0x0004)
// CC3 interrupt pending
#define TIM_SR_CC3IF ((uint16_t)0x0008)
// CC4 interrupt pending
#define TIM_SR_CC4IF ((uint16_t)0x0010)

// USART defines
// Read data register full flag
#define USART_SR_RXNE ((uint16_t)0x0020)
// Transmit data register empty flag
#define USART_SR_TXE ((uint16_t)0x0080)

// Reciever enable flag
#define USART_CR1_RE ((uint16_t)0x0004)
// Transmitter enable flag
#define USART_CR1_TE ((uint16_t)0x0008)
// Receive interrupt enable flag
#define USART_CR1_RXNEIE ((uint16_t)0x0020)
// Transmit data register empty interrupt enable flag
#define USART_CR1_TXEIE ((uint16_t)0x0080)
// Parity selection
#define USART_CR1_PS ((uint16_t)0x0200)
// Parity control
#define USART_CR1_PCE ((uint16_t)0x0400)
// Word length flag
#define USART_CR1_M ((uint16_t)0x1000)
// USART enable flag
#define USART_CR1_UE ((uint16_t)0x2000)

// Last bit-clock send flag
#define USART_CR2_LBCL ((uint16_t)0x0100)
// Clock phase
#define USART_CR2_CPHA ((uint16_t)0x0200)
// Clock polarity
#define USART_CR2_CPOL ((uint16_t)0x0400)
// Clock enable
#define USART_CR2_CLKEN ((uint16_t)0x0800)
// Stop bit 0
#define USART_CR2_STOP0 ((uint16_t)0x1000)
// Stop bit 1
#define USART_CR2_STOP1 ((uint16_t)0x2000)

// Flow control RTS enable
#define USART_CR3_RTSE ((uint16_t)0x0100)
// Flow control CTS enable
#define USART_CR3_CTSE ((uint16_t)0x0200)

// USB defines
// Endpoint correct transfer RX
#define USB_EP_CTR_RX 0x8000
// Endpoint data toggle RX
#define USB_EP_DTOG_RX 0x4000
// Endpoint RX status mask
#define USB_EP_STAT_RX_MASK 0x3000
// Endpoint RX disabled
#define USB_EP_STAT_RX_DISABLED 0x0000
// Endpoint RX stalled
#define USB_EP_STAT_RX_STALL 0x1000
// Endpoint RX no acknowledge
#define USB_EP_STAT_RX_NAK 0x2000
// Endpoint RX valid
#define USB_EP_STAT_RX_VALID 0x3000
// Endpoint setup flag
#define USB_EP_SETUP 0x0800
// Endpoint type mask
#define USB_EP_EP_TYPE_MASK 0x0600
// Bulk endpoint type
#define USB_EP_EP_TYPE_BULK 0x0000
// Control endpoint type
#define USB_EP_EP_TYPE_CONTROL 0x0200
// Isochronous endpoint type
#define USB_EP_EP_TYPE_ISO 0x0400
// Interrupt endpoint type
#define USB_EP_EP_TYPE_INTERRUPT 0x0600
// Endpoint kind
#define USB_EP_EP_KIND 0x0100
// Endpoint correct transfer TX
#define USB_EP_CTR_TX 0x0080
// Endpoint data toggle TX
#define USB_EP_DTOG_TX 0x0040
// Endpoint TX status mask
#define USB_EP_STAT_TX_MASK 0x0030
// Endpoint TX disabled
#define USB_EP_STAT_TX_DISABLED 0x0000
// Endpoint TX stalled
#define USB_EP_STAT_TX_STALL 0x0010
// Endpoint TX no acknowledge
#define USB_EP_STAT_TX_NAK 0x0020
// Endpoint TX valid
#define USB_EP_STAT_TX_VALID 0x0030
// Endpoint address field
#define USB_EP_EA 0x000F
// Bit set if the endpoint is an input endpoint
#define USB_EP_IN 0x80

// USB correct transfer
#define USB_CNTR_CTRM 0x8000
// USB DMA memory overflow
#define USB_CNTR_PMAOVERM 0x4000
// USB error
#define USB_CNTR_ERRM 0x2000
// USB wakeup
#define USB_CNTR_WKUPM 0x1000
// USB suspend
#define USB_CNTR_SUSPM 0x0800
// USB reset
#define USB_CNTR_RESETM 0x0400
// USB start of frame
#define USB_CNTR_SOFM 0x0200
// USB end start of frame
#define USB_CNTR_ESOFM 0x0100
// USB resume
#define USB_CNTR_RESUME 0x0010
// USB force suspend
#define USB_CNTR_FSUSP 0x0008
// USB low power mode
#define USB_CNTR_LP_MODE 0x0004
// USB pwoerdown
#define USB_CNTR_PDWN 0x0002
// USB force reset
#define USB_CNTR_FRES 0x0001

// USB correct transfer flag
#define USB_ISTR_CTR 0x8000
// USB DMA memory overflow flag
#define USB_ISTR_PMAOVER 0x4000
// USB error flag
#define USB_ISTR_ERR 0x2000
// USB wakeup flag
#define USB_ISTR_WKUP 0x1000
// USB suspend flag
#define USB_ISTR_SUSP 0x0800
// USB reset flag
#define USB_ISTR_RESET 0x0400
// USB start of frame flag
#define USB_ISTR_SOF 0x0200
// USB expected start of frame flag
#define USB_ISTR_ESOF 0x0100
// USB direction flag
#define USB_ISTR_DIR 0x0010
// USB endpoint ID
#define USB_ISTR_EP_ID 0x000F

// USB data plus
#define USB_FNR_RXDP 0x8000
// USB data minus
#define USB_FNR_RXDM 0x4000
// USB clock
#define USB_FNR_LCK 0x2000
// USB start of frame
#define USB_FNR_LSOF 0x1800
// USB frame number
#define USB_FNR_FN 0x07FF

// USB data address EF
#define USB_DADDR_EF 0x0080
// USB data address 6
#define USB_DADDR_ADD6 0x0040
// USB data address 5
#define USB_DADDR_ADD5 0x0020
// USB data address 4
#define USB_DADDR_ADD4 0x0010
// USB data address 3
#define USB_DADDR_ADD3 0x0008
// USB data address 2
#define USB_DADDR_ADD2 0x0004
// USB data address 1
#define USB_DADDR_ADD1 0x0002
// USB data address 0
#define USB_DADDR_ADD0 0x0001

// USB buffer table address
#define USB_BTABLE_BTABLE 0xFFF8

// Disables FAULT interrupts
static inline void __disable_fault_irq() { asm volatile ("cpsid f"); }
// Disables interrupts
static inline void __disable_irq() { asm volatile ("cpsid i"); }
// Holds up subsequent memory accesses until previous accesses finish (Data Memory Barrier)
static inline void __dmb() { asm volatile ("dmb"); }
// Waits for memory accesses to complete before continuing (Data Synchronization Barrier)
static inline void __dsb() { asm volatile ("dsb"); }
// Enables FAULT interrupts
static inline void __enable_fault_irq() { asm volatile ("cpsie f"); }
// Enables interrupts
static inline void __enable_irq() { asm volatile ("cpsie i"); }
// Flushes the pipeline, as if a branch prediction failed (Instruction Synchronization Barrier)
static inline void __isb() { asm volatile ("isb"); }
// Resets the processor
static inline __attribute__ ((noreturn)) void __reset() {
	// Ask for reset
	SCB->AIRCR = SCB_AIRCR_VECTKEY | (SCB->AIRCR & SCB_AIRCR_PRIGROUP) | SCB_AIRCR_RESET;
	__dsb();
	// Wait until reset occurs
	while (1);
}
// Goes to sleep until the next interrupt
static inline void __sleep() { asm volatile ("wfi\n\t" "nop"); }

// This is needed to ensure IRQs are known as such to the compiler
#define IRQ void __attribute__ (( interrupt("IRQ") ))
// Used for library functions that are better off in-line for constant expansion
#define INLINE inline __attribute__ (( always_inline ))
// Used to pack structures for building packets and register settings
#define PACKED __attribute__ (( packed ))

// Prototype all ISR functions
// Reset
IRQ ISR_Reset();
// Non-maskable interrupt
IRQ ISR_NMI();
// Hardware fault
IRQ ISR_HardFault();
// Memory management fault
IRQ ISR_MemManage();
// Bus fault
IRQ ISR_BusFault();
// Illegal instruction fault
IRQ ISR_UsageFault();
// Supervisor call
IRQ ISR_SVC();
// Debug monitor
IRQ ISR_DebugMon();
// Pending supervisor call
IRQ ISR_PendSV();
// System tick
IRQ ISR_SysTick();
// Window watchdog
IRQ ISR_WWDG();
// Power/voltage detection
IRQ ISR_PVD();
// Tamper interrupt
IRQ ISR_TAMPER();
// Real-time clock
IRQ ISR_RTC();
// Flash memory
IRQ ISR_FLASH();
// Reset and clock control
IRQ ISR_RCC();
// External interrupts all Px0 pins
IRQ ISR_EXTI0();
// External interrupts all Px1 pins
IRQ ISR_EXTI1();
// External interrupts all Px2 pins
IRQ ISR_EXTI2();
// External interrupts all Px3 pins
IRQ ISR_EXTI3();
// External interrupts all Px4 pins
IRQ ISR_EXTI4();
// DMA1 channel interrupts
IRQ ISR_DMA1_Channel1();
IRQ ISR_DMA1_Channel2();
IRQ ISR_DMA1_Channel3();
IRQ ISR_DMA1_Channel4();
IRQ ISR_DMA1_Channel5();
IRQ ISR_DMA1_Channel6();
IRQ ISR_DMA1_Channel7();
// ADC 1 or 2
IRQ ISR_ADC1_2();
// USB high priority or CAN1 transmit
IRQ ISR_USB_HP_CAN1_TX();
// USB low priority or CAN1 receive 0
IRQ ISR_USB_LP_CAN1_RX0();
// CAN1 receive 1
IRQ ISR_CAN1_RX1();
// CAN1 SCE
IRQ ISR_CAN1_SCE();
// External interrupts all Px5-Px9 pins
IRQ ISR_EXTI9_5();
// TIM1 break
IRQ ISR_TIM1_BRK();
// TIM1 update
IRQ ISR_TIM1_UP();
// TIM1 trigger
IRQ ISR_TIM1_TRG_COM();
// TIM1 capture/compare
IRQ ISR_TIM1_CC();
// TIM2
IRQ ISR_TIM2();
// TIM3
IRQ ISR_TIM3();
// TIM4
IRQ ISR_TIM4();
// I2C1 event
IRQ ISR_I2C1_EV();
// I2C1 error
IRQ ISR_I2C1_ER();
// I2C2 event
IRQ ISR_I2C2_EV();
// I2C2 error
IRQ ISR_I2C2_ER();
// SPI1
IRQ ISR_SPI1();
// SPI2
IRQ ISR_SPI2();
// USART1
IRQ ISR_USART1();
// USART2
IRQ ISR_USART2();
// USART3
IRQ ISR_USART3();
// External interrupts all Px10-Px15 pins
IRQ ISR_EXTI15_10();
// RTC alarm
IRQ ISR_RTCAlarm();
// USB wakeup
IRQ ISR_USBWakeUp();
#ifdef MAPLE_RET6
// TIM8 break
IRQ ISR_TIM8_BRK();
// TIM8 update
IRQ ISR_TIM8_UP();
// TIM8 trigger
IRQ ISR_TIM8_TRG_COM();
// TIM8 capture/compare
IRQ ISR_TIM8_CC();
// ADC3
IRQ ISR_ADC3();
// Flexible static memory controller
IRQ ISR_FSMC();
// Secure digital I/O
IRQ ISR_SDIO();
// TIM5
IRQ ISR_TIM5();
// SPI3
IRQ ISR_SPI3();
// UART4
IRQ ISR_UART4();
// UART5
IRQ ISR_UART5();
// TIM6
IRQ ISR_TIM6();
// TIM7
IRQ ISR_TIM7();
// DMA2 channel interrupts
IRQ ISR_DMA2_Channel1();
IRQ ISR_DMA2_Channel2();
IRQ ISR_DMA2_Channel3();
IRQ ISR_DMA2_Channel4_5();
#endif

// End C++ extern to C
#ifdef __cplusplus
}
#endif

#endif

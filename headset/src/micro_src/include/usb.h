/**
 ******************************************************************************
 * @file    usbd_cdc_vcp.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    22-July-2011
 * @brief   Header for usbd_cdc_vcp.c file.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

#ifndef USB_H_
#define USB_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * RX buffer size, must be a power of two big enough to satisfy:
 *
 * APP_RX_DATA_SIZE * 8000 / MAX_BAUD_RATE should be greater than CDC_IN_FRAME_INTERVAL
 */
#define APP_RX_DATA_SIZE 1024

/**
 * Initializes and configures the USB VCOM port. If this is not used, calls to standard I/O
 * functions on the stdin/stdout streams will produce undefined behavior.
 */
void usbVCPInit(void);
/**
 * Receives up to size bytes into buffer. This function is direct and much, much faster than
 * looping on fgetc(), therefore should be used when receiving lots of data over USB VCP.
 *
 * @param buffer the buffer to store collected bytes
 * @param size the maximum number of bytes to store
 * @return the number of bytes actually stored
 */
uint32_t usbVCPRead(uint8_t *buffer, uint32_t size);
/**
 * Writes up to size bytes of data from buffer. This function is direct and much, much faster
 * than looping on fputc(), therefore should be used when sending lots of data over USB VCP.
 *
 * @param buffer the buffer to send
 * @param size the number of bytes to send
 * @return the number of bytes actually sent
 */
uint32_t usbVCPWrite(uint8_t *buffer, uint32_t size);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

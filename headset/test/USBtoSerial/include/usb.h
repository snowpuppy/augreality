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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported typef ------------------------------------------------------------*/
/* The following structures groups all needed parameters to be configured for the
 ComPort. These parameters can modified on the fly by the host through CDC class
 command class requests. */
typedef struct {
	uint32_t bitrate;
	uint8_t format;
	uint8_t paritytype;
	uint8_t datatype;
} LINE_CODING;

/* Exported constants --------------------------------------------------------*/
#define DEFAULT_CONFIG                  0
#define OTHER_CONFIG                    1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  VCP_BytesAvailable
 *         Counts the number of bytes in the USB receive buffer
 * @retval The number of bytes that can be read from USB
 */
uint32_t VCP_BytesAvailable(void);
/**
 * @brief  VCP_DataTx
 *         CDC received data to be send over USB IN endpoint are managed in
 *         this function.
 * @param  Buf: Buffer of data to be sent
 * @param  Len: Number of data to be sent (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
 */
uint16_t VCP_DataTx(uint8_t* Buf, uint32_t Len);
/**
 * @brief  VCP_GetByte
 *         Reads and returns a byte from USB, returning 0 if no data is available
 * @retval The next byte from the USB-serial COM port
 */
uint8_t VCP_GetByte(void);
/**
 * @brief  VCP_Init
 *         Initializes and configures a USB VCOM port
 * @retval None
 */
void VCP_Init(void);
/**
 * @brief  VCP_SetUSBTxBlocking
 *         Set USB blocking mode for output buffer overrun
 * @param  Mode: 0: non blocking, 1: blocking
 * @retval None
 */
void VCP_SetUSBTxBlocking(uint8_t Mode);

#endif /* __USBD_CDC_VCP_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

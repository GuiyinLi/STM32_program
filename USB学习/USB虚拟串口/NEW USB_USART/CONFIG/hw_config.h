/**
  ******************************************************************************
  * @file    hw_config.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_USART_TXFIFO_SIZE   1024	//USB虚拟串口发送FIFO大小		
#define USB_USART_REC_LEN	200	//USB串口接收缓存区最大字节数
//定义一个USB USART FIFO结构体
typedef struct  
{								    
	u8  buffer[USB_USART_TXFIFO_SIZE];	//buffer
	vu16 writeptr;			//写指针
	vu16 readptr;			//读指针
}_usb_usart_fifo; 
extern _usb_usart_fifo uu_txfifo;		//USB串口发送FIFO
extern u8  USB_USART_RX_BUF[USB_USART_REC_LEN]; //接受缓存,最大USB_USART_REC_LEN个字节  
extern u16 USB_USART_RX_STA;   		//接收状态标记

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void USART_Config_Default(void);
bool USART_Config(void);
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint8_t Nb_bytes);
void USART_To_USB_Send_Data(void);
void Handle_USBAsynchXfer (void);
void Get_SerialNum(void);

void IntToUnicode (u32 value,u8 *pbuf,u8 len);
void USB_USART_SendData(u8 data);
void USB_Port_Set(u8 enable);
void usb_printf(char* fmt,...); 
/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

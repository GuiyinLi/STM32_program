/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
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


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern  uint8_t USART_Rx_Buffer[];
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;
extern uint8_t  USB_Tx_State;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
	u16 USB_Tx_ptr; u16 USB_Tx_length; 
	if(uu_txfifo.readptr==uu_txfifo.writeptr)return;	//无任何数据要发送，直接退出
	if(uu_txfifo.readptr<uu_txfifo.writeptr)		//没有超过数组,读指针<写指针
	{
		USB_Tx_length=uu_txfifo.writeptr-uu_txfifo.readptr;	//得到要发送数据的长度
	}else USB_Tx_length=USB_USART_TXFIFO_SIZE-uu_txfifo.readptr;//读指针>写指针时发送的数据长度
	if(USB_Tx_length>VIRTUAL_COM_PORT_DATA_SIZE)	//超过64字节
	{
		USB_Tx_length=VIRTUAL_COM_PORT_DATA_SIZE;//此次发送的数据量
	}
	USB_Tx_ptr=uu_txfifo.readptr;		//发送起始地址
	uu_txfifo.readptr+=USB_Tx_length;		//读指针偏移
	if(uu_txfifo.readptr>=USB_USART_TXFIFO_SIZE)	//读指针归零
	{
		uu_txfifo.readptr=0;
	} 
	UserToPMABufferCopy(&uu_txfifo.buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);//考培数据
	SetEPTxCount(ENDP1, USB_Tx_length); //设置端点1发送数据长度
	SetEPTxValid(ENDP1); //设置端点1发送有效
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  uint16_t USB_Rx_Cnt;
  
  /* Get the received data buffer and update the counter */
  USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
  
  /* USB data will be immediately processed, this allow next USB traffic being 
  NAKed till the end of the USART Xfer */
  
  USB_To_USART_Send_Data(USB_Rx_Buffer, USB_Rx_Cnt);
 
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//帧首信号回调函数,对全速设备每1ms有1帧首信号
void SOF_Callback(void)
{
	static uint32_t FrameCount = 0;
	if(bDeviceState == CONFIGURED)
	{
		if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
		{ 
			FrameCount = 0;//重设帧计数器
			EP1_IN_Callback();//通过EP1_IN_Callback函数实现TX数据发送给USB 
		}
	}  
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


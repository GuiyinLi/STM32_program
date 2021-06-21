#include "usart2.h"
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  

//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
                                                 //__align()用于修改最高级别对象的字节边界 来保证数据对象是相应对齐
#ifdef USART2_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)         //接收到数据
	{
		res=USART_ReceiveData(USART2);
		if(USART2_RX_STA<2)                                      //还能接收数据
		{
			USART2_RX_BUF[USART2_RX_STA&0x7fff]=res;                      //记录接收到的值
			USART2_RX_STA++;
		}
		else
			USART2_RX_STA|=1<<15;                               //超出最大字节 强制接收完成
	}
}
//发送一个字节
void USART2_SendByte(u8 Data)
{
   //等待发送数据寄存器空
   while (!(USART2->SR & USART_FLAG_TXE));
   //发送数据
   USART2->DR = (Data & 0x00FF);
   //等待发送完成
   while (!(USART2->SR & USART_FLAG_TC));   
}
//发送多个字节数据
void USART2_Write(u8* data,u16 len)
{
	u16 i;

	for (i=0; i<len; i++)
	  USART2_SendByte(data[i]);	

}

//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_DeInit(USART2);                                //复位串口2
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //PA2 TX
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;           //PA3 RX
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=bound;                 //一般为9600
	USART_InitStructure.USART_WordLength=USART_WordLength_8b; //字长为8位数据长度
	USART_InitStructure.USART_StopBits=USART_StopBits_1;      //一个停止位
	USART_InitStructure.USART_Parity=USART_Parity_No;         //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;               //收发模式
	USART_Init(USART2,&USART_InitStructure);
	
	USART_Cmd(USART2,ENABLE);                                   //使能串口2
#ifdef USART2_RX_EN		  	//如果使能了接收

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断  
	
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;   //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;         //相应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}
#endif


#include "usart2.h"

u8 USART2_RX_BUF[USART2_REC_LEN]; //数组用来存储接收到的数据，而USART3_REC_LEN为最多能接收的字节限度
u16 USART2_RX_STA=0;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位
u16 len;                        //记录数组长度
u8 flag=0;                      //数组接收完成标志位
void usart2_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//串口二对应引脚复用
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;       //复用推完输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;   //速度 100MHZ
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=bound;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;  //无硬件数据流
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;                //无奇偶校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;              //一个停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;          //8位数据
	USART_Init(USART2,&USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);  //使能串口2
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
	
	//Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

u16 len;
u8 flag;
void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)         //接收到数据
	{
		res=USART_ReceiveData(USART2);
		if(!flag)
		{
			USART2_RX_BUF[USART2_RX_STA&0x7fff]=res;                      //记录接收到的值
			len=(++USART2_RX_STA)&0x7fff;
		}
		if(USART2_RX_BUF[0]!=0xaa)
			USART2_RX_STA=0;
		else
		{
			if(res==0x55)
			{
				flag=1;                      //先停止数组的接收
				USART2_RX_STA|=0x8000;
			}
		}
		if((USART2_RX_STA&0x7fff)>USART2_REC_LEN)
			USART2_RX_STA|=0x8000;                        //强制接收完成
		USART_ClearITPendingBit(USART2,USART_IT_RXNE );
	}
}






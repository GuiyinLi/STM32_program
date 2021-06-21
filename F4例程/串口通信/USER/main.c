#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "usart2.h"
#include "can.h"
int main(void)
{ 
	u8 i,t;
	u8 canbuf[8];
	delay_init(168);		  //初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //设置为第2组  2位抢占 2位响应
	LED_Init();		        //初始化LED端口
	usart2_init(115200);
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);  //500Kpbs 环回模式
	while(1)
	{
		if(USART2_RX_STA&0x8000)
		 {
			 Can_Send_Msg(USART2_RX_BUF,len);     //将获取的数据通过CAN发送给自身
			 USART2_RX_STA=0;
			 flag=0;
		 }//CAN一次性最多能发送/接收8个数据
//		 t=Can_Receive_Msg(canbuf);
//		 if(t)  //接收到数据
//		 {
//			 for(i=0;i<t;i++)
//			 {
//				 USART_SendData(USART2,canbuf[i]);		
//				 while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束		该句不能省！！		 
//			 }
//		 }
	}
}


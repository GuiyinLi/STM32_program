#include "stm32f10x.h" 
#include "sys.h" 
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "oled.h"
#include "key.h"

//发送命令字节
//命令格式：0xA5+command+sum
void send_com(u8 data)   //发送命令字节
{
	u8 bytes[3]={0};
	bytes[0]=0xa5;
	bytes[1]=data;
	USART_Send(bytes,3);         //发送帧头 命令 校验和
}
 int main(void)
{
	u8 sum,i;  //sum用于判断校验和
	u16 distance;
	u8 k;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	                         //串口初始化为9600
	LED_Init();
	delay_init();
	KEY_Init();
	OLED_GPIO_Init();                            //OLED初始化
  OLED_Init();
	delay_ms(500);                            //等待模块初始化完成
	send_com(0x45);//发送读rgb指令  设置为连续输出距离数据
	led=1;
	OLED_P8x16Str(0,0,"Mode:");
	OLED_P8x16Str(0,2,"KEY0:normal");
	OLED_P8x16Str(0,4,"WK_UP:fast");
	OLED_P8x16Str(32,6,"mm");
	while(1)
	{
//		if(Receive_ok)  //串口接收完毕
//		{
//			for(sum=0,i=0;i<(re_Buf_Data[3]+4);i++) //rgb_data[3]=3   [3]+4:接收的总数据量
//			{
//				sum+=re_Buf_Data[i];
//			  if(sum==re_Buf_Data[i])//校验和判断   //Byte7前面数据累加和
//			  {
//			  	distance=re_Buf_Data[4]<<8|re_Buf_Data[5];   //计算距离  单位为mm
//					OLED_ShowNum(0,2,distance,4,16);
//					led=!led;
//			  }
//			  Receive_ok=0;     //数据接收处理完毕
//		  } 
//	  }
		k=KEY_Scan(0);     //不支持连续按
		if(k==KEY0_PRES)
		{
			send_com(0x53);        //高精度测量
		}
		if(k==WK_UP_PRES)
		{
			send_com(0x51);        //快速测量
		}
		if(USART_RX_STA&0x8000)
		{
			if((USART_RX_BUF[0]==0x5a)&&(USART_RX_BUF[1]==0x5a))
			{
				distance=USART_RX_BUF[4]<<8|USART_RX_BUF[5];   //计算距离  单位为mm
			  OLED_ShowNum(0,6,distance,4,16);
				switch (USART_RX_BUF[6])
				{
					case 0x03:
						OLED_P8x16Str(40,0,"normal");
					  break;
					case 0x02:
						OLED_P8x16Str(40,0,"high  ");
					  break;
					case 0x01:
						OLED_P8x16Str(40,0,"fast  ");
					  break;
					case 0x00:
						OLED_P8x16Str(40,0,"long  ");
					  break;
				}
			}
			USART_RX_STA=0;
		}
  }
}



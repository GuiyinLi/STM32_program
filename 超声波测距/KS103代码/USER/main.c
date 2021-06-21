#include "stm32f10x.h" 
#include "sys.h" 
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "iic.h"
#include "ks103.h"
#include "usart2.h"
#include "key.h"
#include "string.h"
int main(void)
{
	u8 k;
	u8 flag=0;
	u32 distance=0;
	delay_init();
	LED_Init();
	Ks103_Init();
	KEY_Init();
	Ks103_Write_byte(SlaveAddress1,Res_2,Noise_Reduction_2);//一级降噪
	delay_ms(2000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);
	USART2_Init(9600);                            //KS103默认模特率 9600
	while(1)
	{
		k=KEY_Scan(0);
		if(k==KEY0_PRES) flag=!flag;  //默认无温度补偿  PB5在开启NVIC中断后不可用
		/*IIC模式*/
		if(flag)
		{
			if(Read_Ks103_Data(SlaveAddress1,Temp0_5m))
			{
				distance=Read_Ks103_Data(SlaveAddress1,Temp0_5m);
		    printf("IIC_YT:distance=%dmm\r\n",distance);
			}
		}
		else
		{
			if(distance!=Read_Ks103_Data(SlaveAddress1,Notemp0_5m))   //判断获取值是否正确
			{
				distance=Read_Ks103_Data(SlaveAddress1,Notemp0_5m);
				printf("IIC_NT:distance=%dmm\r\n",distance);
			}
	  }
		/*串口模式 暂时不可用*/
		/*Send_Command(Notemp0_5m);         //发送距离探测命令
		if(USART_RX_STA&0x8000)
		{
			distance=USART_RX_BUF[0]<<8|USART_RX_BUF[1];
			printf("TTL:distance=%dmm\r\n",distance);
			USART_RX_STA=0;
			memset(USART_RX_BUF,0,strlen((char *)USART_RX_BUF));
		}*/
	}

}



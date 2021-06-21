#include <reg52.h>
#include "usart.h"
#include "iic.h"  
/*
硬件接法：
GY-53----C51
PWM_input---P3^7
C51---FT232
TX ---RX
RX ---TX
软件说明：
该程序采用PWM进行读取数据 ,距离=高电平时间(us)/10

联系方式：
http://shop62474960.taobao.com/?spm=a230r.7195193.1997079397.2.9qa3Ky&v=1
*/
void TIME0_Int(uint32_t BaudRatePrescaler)
{
	 TMOD |= 0x01;//定时器0，工作方式1，16位计数
     TH0   =0; 
	 TL0   =0;
	 ET0 =0;
  
}
void delay(unsigned int x)
{
	while(x--);
}
sbit PWM_pin=P3^7;
int main(void)
{
	unsigned char  raw_data[9]={0};
	unsigned int buf=0,distance=0;
	Usart_Int(9600);   
	TIME0_Int();
	while(PWM_pin);	 //等待拉高
 	while(1)
	{
	  while(!PWM_pin); //等待拉低，启动定时
	  ET0=1;
	  TR0=1;
	  while(PWM_pin);//等待拉高，关闭定时
	  TR0=0x0;
	  ET0=0x0;

	  buf=(unsigned int)TH0<<8|TL0;	 //读取定时器计数
	  distance=(float)buf/110.592*12;//数据转换为距离，定时器计一次时间为12/11.0592（晶振）us，距离=时间(us)/10
	  raw_data[0] =distance>>8;
	  raw_data[1] =	distance;
	  raw_data[2] =2; //模块默认模式
	  send_3out(raw_data,3,0x15);//上传到上位机
	   TH0   =0x0; 
	   TL0   =0x0;
	 
	}
}

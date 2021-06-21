#include <reg52.h>
#include "usart.h"
#include "iic.h"  
/*
硬件接法：
GY-53---c51
1、GY-53_RX---c51_TX,c51复位将发送A5 45 EA 给模块
2、c51_TX---FT232,STM32将数据上传给上位机
3、GY-53_TX---c51_RX，接收模块角度数据
软件说明:
该程序采用串口方式获取模块数据，波特率9600

注：中断函数位于stc_it.c
联系方式：
http://shop62474960.taobao.com/?spm=a230r.7195193.1997079397.2.9qa3Ky&v=1
*/
void send_com(u8 datas)
{
	u8 bytes[3]={0};
	bytes[0]=0xa5;
	bytes[1]=datas;//功能字节
	USART_Send(bytes,3);//发送帧头、功能字节、校验和
}

int main(void)
{

    u8 sum=0,i=0;
	uint16_t distance=0;
	Usart_Int(9600);
	send_com(0x45);//发送读方位角指令
 	while(1)
	{
		if(Receive_ok)//串口接收完毕
		{
			for(sum=0,i=0;i<(re_Buf_Data[3]+4);i++)//rgb_data[3]=3
			sum+=re_Buf_Data[i];
			if(sum==re_Buf_Data[i])//校验和判断
			{
				distance=re_Buf_Data[4]<<8|re_Buf_Data[5];
				send_3out(&re_Buf_Data[4],3,0x15);//上传给上位机
			}
			Receive_ok=0;//处理数据完毕标志
		}	
	}
}

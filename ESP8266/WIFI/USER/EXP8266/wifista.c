#include "wifista.h"
#include "gpio.h"
uint8_t Wifi_Client_Send_Flag=0;
//STA模式下连接到TCP服务器
//ipbuf:IP地址 portnum:端口号
//返回值: 0:成功 1：失败
int Sta_Connect(uint8_t* ipbuf,uint8_t* portnum )
{
	int times=0;
	printf("TCP_Connect Message:\r\n");
	memset(USART2_TX_BUF,0,BUFFLEN);
	sprintf((char*)USART2_TX_BUF,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,portnum);    //配置目标TCP服务器
	while(!atk_8266_send_cmd((uint8_t *)USART2_TX_BUF,"OK",100)) 
	{
    times++;
		if(times==5)
			break;
	}
	if(times<5)
	{
		printf("TCP Connect Successed\r\n");
	  LED1_ON();
	}
	else
	{
		printf("TCP Connect Failed\r\n");
		return 1;
	}
	printf("\r\n");
  return 0;	
}

//TCP发送信息
//mode 0：作为客户端发送信息 1：作为服务器发送信息
void TCP_Client_Send(uint8_t *p)
{
	int times=0;
	printf("Data Send Message:\r\n");
	if(atk_8266_send_cmd("AT+CIPMODE=1","OK",100)) printf("Enter Clear Trans  Failed\r\n");//传输模式为：透传	
	else printf("Enter Clear Trans  Successed\r\n");


		if(!atk_8266_send_cmd("AT+CIPSEND",">",300))  //开启发送 
		{
			printf("Wait Send Data......\r\n");
			atk_8266_send_data(p);
			printf("Data Send Success\r\n");  //发送数据成功
		}
		else 
		{
			printf("Data Send Failed\r\n");  //发送数据失败·
			printf("Unopen Send!\r\n");
			if(atk_8266_consta_check()) 
			{
				printf("TCP Connect Failed\r\n");//得到连接状态
				Sta_Connect("10.164.154.231","8080");
			}
			else printf("TCP Connect Successed\r\n");
		}
	
	if(!atk_8266_quit_trans()) 
	{
		printf("Exit Clear Trans  Successed\r\n");   //发送完数据之后退出透传 进入AT指令模式 方便进行其他操作
	}
  else printf("Exit Clear Trans  Failed\r\n");
	printf("\r\n");
}
	
void Wifi_Sta_Init()
{
	if(WIFI_Init(1,0,"Honor 10","1893193446"));  //连接无线网
	{
		HAL_Delay(8000);   //等待联网  时间不要太短
	  //Sta_Connect("10.164.154.231","8080");   //连接服务器
	}
	AP_STA_MODE=1;
	memset(USART2_RX_BUF,0,BUFFLEN);  //清空接收数组
	USART2_RX_STA=0;
	
}




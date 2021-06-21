#include "wifiap.h"
#include "gpio.h"
uint8_t Wifi_Server_Send_Flag=0;
// 名称 密码 端口
//0:成功 1:失败
uint8_t Set_Server(uint8_t* SSID,uint8_t* PASS,uint8_t* portnum )
{
	printf("Set Up Server Message:\r\n");
	
	memset(USART2_TX_BUF,0,BUFFLEN);
	sprintf((char*)USART2_TX_BUF,"AT+CWSAP=\"%s\",\"%s\",1,4",SSID,PASS);//建立服务器 通道号为1 加密方式为WPA2 PSK
	if(!atk_8266_send_cmd(USART2_TX_BUF,"OK",100))
		printf("建立服务器成功\r\n");
	else 
		return 1;
	
	/*if(!atk_8266_send_cmd("AT+CIPMUX=1","OK",100))
		printf("开启多连接成功\r\n");    //要设置为服务器模式 必须开启多连接
	else
		return 1;*/
	
	memset(USART2_TX_BUF,0,BUFFLEN);
	sprintf((char*)USART2_TX_BUF,"AT+CIPSERVER=1,%s",portnum);//开启Server 设置端口号
	if(!atk_8266_send_cmd(USART2_TX_BUF,"OK",100))
		printf("设置端口号成功\r\n");
	else
		return 1;
	printf("Server Message:\r\n");
	printf("IP: 192.168.4.1  SSID: %s  PASS: %s  portnum: %s\r\n",SSID,PASS,portnum);
	printf("\r\n");
	LED1_ON();
	return 0;
}

void Wifi_Ap_Init()
{
	if(WIFI_Init(2,1,"Honor 10","1893193446"));  //连接无线网
	{
		HAL_Delay(8000);
	  Set_Server("ATK-ESP8266","1659823781","8086");   //连接服务器
	}
	AP_STA_MODE=0;
	memset(USART2_RX_BUF,0,BUFFLEN);  //清空接收数组
	USART2_RX_STA=0;
}

//TCP发送信息
//数据 长度 ID号
void TCP_Server_Send(uint8_t *p,uint8_t id)
{
	int times=0;
	uint8_t length=0;
	printf("Data Send Message:\r\n");
	if(atk_8266_send_cmd("AT+CIPMODE=1","OK",100)) printf("Enter Clear Trans  Failed\r\n");//传输模式为：透传	
	else printf("Enter Clear Trans  Successed\r\n");
	length=strlen((const char*)p);
  memset(USART2_TX_BUF,0,BUFFLEN);
	sprintf((char*)USART2_TX_BUF,"AT+CIPSEND=%d,%d",id,length);//开启Server 设置端口号
  
		if(!atk_8266_send_cmd(USART2_TX_BUF,"OK",100))  //开启发送 
		{
			printf("Wait Send Data......\r\n");
			atk_8266_send_data(p);
			printf("Data Send Success\r\n");  //发送数据成功
		}
		else 
		{
			printf("Data Send Failed\r\n");  //发送数据失败·
		}
	
	if(!atk_8266_quit_trans()) 
	{
		printf("Exit Clear Trans  Successed\r\n");   //发送完数据之后退出透传 进入AT指令模式 方便进行其他操作
	}
  else printf("Exit Clear Trans  Failed\r\n");
	printf("\r\n");
}
	

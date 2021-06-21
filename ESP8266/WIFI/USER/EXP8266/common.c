#include "common.h"
#include "wifista.h"
#include "gpio.h"
#include "stdio.h"
uint8_t AP_STA_MODE=0;  //0:AP 1:STA
uint8_t AP_STA_SWITCH=0;
//连接端口号:8086,可自行修改为其他端口.
const uint8_t* portnum="8086";		 

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const uint8_t* wifista_ssid="Honor 10";			//路由器SSID号
const uint8_t* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const uint8_t* wifista_password="18931983446"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
const uint8_t* wifiap_ssid="ATK-ESP8266";			//对外SSID号
const uint8_t* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const uint8_t* wifiap_password="12345678"; 		//连接密码 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//5种加密方式
const uint8_t *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(uint8_t mode)
{
	if(USART2_RX_STA)		//接收到一次数据了
	{ 
		USART2_RX_BUF[RX_LEN]=0;//添加结束符
		printf("%s",USART2_RX_BUF);	//发送到串口
		if(mode)USART2_RX_STA=0;
	} 
}

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t* atk_8266_check_cmd(uint8_t *str)
{
	
	char *strx=0;
	if(USART2_RX_STA)		//接收到一次数据了
	{ 
		USART2_RX_BUF[RX_LEN]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);//从a字符串里查看是否有b字符串 有则从首次发现b字符串处返回a字符串
		memset(USART2_RX_BUF,0,BUFFLEN);  //清空接收数组
		USART2_RX_STA=0;
	} 
	return (uint8_t*)strx;
}

//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)  back:0不反回接收信息 1:返回接受信息
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART2_RX_STA=0;
	u2_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			HAL_Delay(10);//10ms
			if(USART2_RX_STA)//接收到期待的应答结果
			{
				
				if(atk_8266_check_cmd(ack))
				{
					
					printf("ack:%s\r\n",(uint8_t*)ack);
					break;//得到有效数据 
				}
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

//向ATK-ESP8266发送指定数据
//函数封装
void atk_8266_send_data(uint8_t *data)
{
	u2_printf("%s",data);	//发送命令
	HAL_Delay(20);   //此处要有延时 否则会和退出透传指令冲突
}

//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
uint8_t atk_8266_quit_trans(void)
{
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	HAL_Delay(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	HAL_Delay(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	HAL_Delay(500);					//等待500ms
	return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}

//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
uint8_t atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}

//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
uint8_t atk_8266_consta_check(void)
{
	uint8_t res;
	if(atk_8266_quit_trans()) return 1;			//退出透传 
	res=atk_8266_send_cmd("AT+CIPSTATUS","3",50);	//发送AT+CIPSTATUS指令,查询连接状态
	return res;
}

//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(uint8_t* ipbuf)
{
	uint8_t *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");
		p1=(uint8_t*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}

//WIFI模块初始化函数
//work_mode:模式选择  1:STA模式(默认TCP连接 仅在此模式下可以开启透传) 2:AP模式 3:AP+STA模式
//con_mode:连接模式选择 0：单连接，1：多连接
//wifista_ssid:无线网名称 wifista_password:密码
//返回值 0：失败 1:成功
int WIFI_Init(int work_mode,int con_mode,uint8_t* wifista_ssid,uint8_t* wifista_password)
{
	int times=0;
	atk_8266_send_cmd("AT+RST","OK",20);  //每次上电进行重启 防止ESP8266因繁忙而无法回复信息
	printf("Init Message:\r\n");
	while(atk_8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
	{
		times++;
		atk_8266_quit_trans();//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		HAL_Delay(1000);
		if(times>1) break;
	} 
	if(times>1) 
	{
		printf("No WIFI\r\n");
		return 0;
	}
	if (atk_8266_send_cmd("ATE0","OK",20))  //关闭回显
	{
		printf("Close Back Display Failed\r\n");
		return 0;
	}
	else printf("Close Back Display Successed\r\n");
	switch (work_mode)
	{
		case 1:  if(!atk_8266_send_cmd("AT+CWMODE=1","OK",50)) printf("Mode:STA\r\n");break;		//设置WIFI STA模式
		case 2:  if(!atk_8266_send_cmd("AT+CWMODE=2","OK",50)) printf("Mode:AP\r\n");break;		//设置WIFI AP模式
		case 3:  if(!atk_8266_send_cmd("AT+CWMODE=3","OK",50)) printf("Mode:STA+AP\r\n");break;		//设置WIFI AP+STA模式
	}
	if(work_mode!=1)
	{
	  if(atk_8266_send_cmd("AT+RST","OK",20)) return 0;		//DHCP服务器关闭(仅AP模式有效) 
		else printf("Wait Rst\r\n");
	  HAL_Delay(3000);   //等待重启
	}
	
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	if(work_mode!=2)  //非AP模式下才可进行联网
	{
	
			memset(USART2_TX_BUF,0,BUFFLEN);
			sprintf((char*)USART2_TX_BUF,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
			if(atk_8266_send_cmd(USART2_TX_BUF,"WIFI GOT IP",300))   //返回值是？？ 
			{
				printf("Wifi Already  Connected\r\n");
			}
			else 
			{
				printf("WIFI Connect Successed\r\n");
				printf("WIFI Message:\r\n");
				atk_8266_send_cmd("AT+CIFSR","+",10);
			}
	}
	
	if(con_mode==0)
	{
	  if(atk_8266_send_cmd("AT+CIPMUX=0","OK",20)) 
		{
			printf("Set Connect_mode Failed\r\n");
			return 0;   //单连接
		}
		else printf("Connect_mode: Single connect\r\n");
	}
  else if(con_mode==1)
	{
			if(atk_8266_send_cmd("AT+CIPMUX=1","OK",20)) 
			{
				printf("Set Connect_mode Failed\r\n");
				return 0;   //多连接
			}
		  else printf("Connect_mode: Serval connect\r\n");
	}
	printf("WIFI Init Successed\r\n");
	printf("\r\n");
	LED0_ON();
	return 1;
}

//模式转换
void Mode_Switch()
{
	if(AP_STA_SWITCH)
		{
			if(AP_STA_MODE==0)
			{
			  printf("START SWITCH  AP->STA\r\n");
			  if(!atk_8266_send_cmd("AT+RST","OK",20))
		     printf("Wait Rst\r\n");
	      HAL_Delay(3000);   //等待重启
			  Wifi_Sta_Init();
			}
			else
			{
				printf("START SWITCH  SAT->AP\r\n");
				if(!atk_8266_send_cmd("AT+RST","OK",20)) 
		     printf("Wait Rst\r\n");
	      HAL_Delay(3000);   //等待重启
				Wifi_Ap_Init();
			}
			AP_STA_SWITCH=0;
		}
}





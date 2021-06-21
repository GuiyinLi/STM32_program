#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "spi.h"
#include "rc522.h"

int main(void)
 {		
	 u8 status,i;
	 	u16 temp;
	 	unsigned char data1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反 
unsigned char data2[4]  = {0,0,0,0x01};
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
	 unsigned char g_ucTempbuf[20]; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	  
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 //串口初始化为115200
	LED_Init();
	SPI2_Init();
	 InitRc522();
  while(1)
	{
		/*
		  status = PcdRequest(PICC_REQALL, g_ucTempbuf);//寻卡
      if (status != MI_OK)
      {      
					PcdReset();
					PcdAntennaOff(); 
					PcdAntennaOn(); 
					continue;
       }		     
//			printf("卡的类型:");
//      for(i=0;i<2;i++)
//			{
//					temp=g_ucTempbuf[i];
//					printf("%X",temp);
//						
//			}
////////////////////////////////////////////////////						
      status = PcdAnticoll(g_ucTempbuf);//防冲撞
      if (status != MI_OK)    
			{			continue;   	}
//			printf("卡序列号：");	//超级终端显示,
//			for(i=0;i<4;i++)
//			{
//					temp=g_ucTempbuf[i];
//				  printf("%X",temp);
//						
//			}
//         printf("\r\n");
///////////////////////////////////////////////////////////
         status = PcdSelect(g_ucTempbuf);//选定卡片
         if (status != MI_OK)
         {    continue;    }
         
         status = PcdAuthState(PICC_AUTHENT1A, 4, DefaultKey, g_ucTempbuf);//验证卡片密码
         if (status != MI_OK)
         {    continue;    }
         
//         status = PcdWrite(4, data1);//写块
//         if (status != MI_OK)
//         {    continue;    }
				 */
while(1)
		{
         status = PcdRequest(PICC_REQALL, g_ucTempbuf);//寻卡
         if (status != MI_OK)
         {   
     PcdReset();
     PcdAntennaOff(); 
     PcdAntennaOn(); 
			  continue;
         }
		 status = PcdAnticoll(g_ucTempbuf);//防冲撞
         if (status != MI_OK)
         {    continue;    }
		status = PcdSelect(g_ucTempbuf);//选定卡片
         if (status != MI_OK)
         {    continue;    }
         
         status = PcdAuthState(PICC_AUTHENT1A, 4, DefaultKey, g_ucTempbuf);//验证卡片密码
         if (status != MI_OK)
         {    continue;    }


//         status = PcdValue(PICC_DECREMENT,4,data2);//扣款
//         if (status != MI_OK)
//         {    continue;    }
//		 
//         status = PcdBakValue(4, 5);//块备份
//         if (status != MI_OK)
//         {    continue;    }
         
         status = PcdRead(4, g_ucTempbuf);//读块
         if (status != MI_OK)
         {    continue;    }
        	//printf("卡读块：");	//超级终端显示,
         for(i=0;i<16;i++)
					{
						temp=g_ucTempbuf[i];
						printf("%X",temp);
						
					}

		 printf("\n");
			 		         
		 PcdHalt();
		}
	}	 
} 


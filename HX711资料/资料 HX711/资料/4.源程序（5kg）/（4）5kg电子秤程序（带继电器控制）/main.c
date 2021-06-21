/**********************************************************************

***********************************************************************/

#include "main.h"
#include "HX711.h"
#include "uart.h"
#include "LCD1602.h"
#include "eeprom52.h"
#define   uchar unsigned char
#define   uint unsigned int
unsigned long HX711_Buffer = 0;
unsigned long Weight_Maopi = 0;
long Weight_Shiwu = 0;
long Weight_warn = 0;

unsigned char flag = 0;
bit Flag_ERROR = 0;
sbit speak= P1^7;	 //蜂鸣器
sbit jiare= P3^0;	//继电器接口
uchar Ncunchu[5];
//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 430



/******************把数据保存到单片机内部eeprom中******************/
void write_eeprom()
{  

	
	Ncunchu[0] = Weight_warn/256;
	Ncunchu[1] = Weight_warn%256;
	SectorErase(0x2000);
	byte_write(0x2011,Ncunchu[0]);
	byte_write(0x2012,Ncunchu[1]);



  byte_write(0x2060, a_a);	
}

/******************把数据从单片机内部eeprom中读出来*****************/
void read_eeprom()
{

	
	Ncunchu[0] = byte_read(0x2011);
	Ncunchu[1] = byte_read(0x2012);
	Weight_warn  = Ncunchu[0]*256+Ncunchu[1];


	
    a_a      = byte_read(0x2060);
}

/**************开机自检eeprom初始化*****************/
void init_eeprom()
{
	read_eeprom();		//先读
	if(a_a != 5)		//新的单片机初始单片机内问eeprom
	{
	   a_a =5;
	   Weight_warn = 500;
       write_eeprom();
	}	
	
	
	
}

//****************************************************
//主函数
//****************************************************
void main()
{
    Uart_Init();
    Send_Word("Welcome to use!\n");
    Send_Word("Made by Beetle Electronic Technology!\n");
	
	Init_LCD1602();
	LCD1602_write_com(0x80);
	LCD1602_write_word("Weight  Warning");
	init_eeprom();      			//读eeprom数据
 
	Delay_ms(1000);		 //延时,等待传感器稳定
	Get_Maopi();				//称毛皮重量
    Delay_ms(100);		 
	Get_Maopi();				//称毛皮重量
	Delay_ms(100);		 
	Get_Maopi();				//多次去皮有利于稳定
	
	while(1)
	{
		EA = 0;
		Get_Weight();			//称重
		EA = 1;

		Scan_Key();

		//显示当前重量
		if( Flag_ERROR == 1)
		{
      Send_Word("ERROR\n");
			
			LCD1602_write_com(0x80+0x40);
			LCD1602_write_word("ERROR ");
			speak=0;
		}		
		else
		{         
			//speak=1;
            LCD1602_write_com(0x80+0x40);
			LCD1602_write_data(Weight_Shiwu/1000 + 0X30);
            LCD1602_write_data(Weight_Shiwu%1000/100 + 0X30);
            LCD1602_write_data(Weight_Shiwu%100/10 + 0X30);
            LCD1602_write_data(Weight_Shiwu%10 + 0X30);
			LCD1602_write_word(" g");

			LCD1602_write_com(0x80+0x40+8);
			LCD1602_write_data(Weight_warn/1000 + 0X30);
            LCD1602_write_data(Weight_warn%1000/100 + 0X30);
            LCD1602_write_data(Weight_warn%100/10 + 0X30);
            LCD1602_write_data(Weight_warn%10 + 0X30);
			LCD1602_write_word(" g");
		}
		
	}
}

//扫描按键
void Scan_Key()
{
	if(KEY1 == 0)
	{
		Delay_ms(5);
		if(KEY1 == 0)
		{
	   	 Get_Maopi();
		}	
	}

	if(KEY2 == 0)
	{
		Delay_ms(20);
		if(KEY2 == 0)
		{
		//	while(KEY2 == 0);
		   if(Weight_warn<=5000)	 { Weight_warn+=1;}
		    write_eeprom();       //保存数据
		}	
	}

	if(KEY3 == 0)
	{
		Delay_ms(20);
		if(KEY3 == 0)
		{
		//	while(KEY3 == 0);
	    	if(Weight_warn>=50)	Weight_warn-=1;
			 write_eeprom();       //保存数据
		}	
	}

  if(Weight_Shiwu<Weight_warn)   {jiare=1; speak=1;}	 //继电器关闭
	if(Weight_Shiwu>=Weight_warn)  {jiare=0; speak=0;}	 //继电器打开
}

//****************************************************
//称重
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//获取净重
	if(Weight_Shiwu > 0)			
	{	
		Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量
																		
																		
		if(Weight_Shiwu > 5000)		//超重报警
		{
			Flag_ERROR = 1;	
		}
		else
		{
			Flag_ERROR = 0;
		}
	}
	else
	{
		Weight_Shiwu = 0;
	//	Flag_ERROR = 1;				//负重报警
	}
	
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//MS延时函数(12M晶振下测试)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}
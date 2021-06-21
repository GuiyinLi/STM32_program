#include "ks103.h"
#include "iic.h"
#include "delay.h"
#include "usart2.h"

void Ks103_Init(void)
{
	IIC_Init();
}
/*************************MODE悬空 IIC模式*******************/
//KS103写命令
//器件地址
//寄存器地址
//命令字节
void Ks103_Write_byte(u8 SlaveAddress,u8 ResAddress,u8 command)
{
	  IIC_Start();                   
    IIC_Send_Byte(SlaveAddress);  
 
    while(IIC_Wait_Ack());
 
    IIC_Send_Byte(ResAddress);   
 
    while(IIC_Wait_Ack());                          
                                  
    IIC_Send_Byte(command);      
 
    while(IIC_Wait_Ack());                                
    IIC_Stop();                    
}

//KS103读数据
//返回读取数据  16位数据
u8 Ks103_Read_byte(u8 SlaveAddress,u8 ResAddress)
{
	  u8 data=0;
	  IIC_Start();    
    IIC_Send_Byte(SlaveAddress);    
 
    while(IIC_Wait_Ack());
    IIC_Send_Byte(ResAddress);
 
    while(IIC_Wait_Ack());                        
    IIC_Start();
    IIC_Send_Byte(SlaveAddress+1);   
     
    while(IIC_Wait_Ack());
 
    delay_us(50);
 
    data=IIC_Read_Byte(0);
 
    IIC_Stop();  
     
    return data;
}


//改变器件地址
void Change_Addr(u8 OldAdddr,u8 NewAddr)
{ 
    delay_ms(2000);
    Ks103_Write_byte(OldAdddr,0x02,0x9a);             //??????0x00;
    delay_ms(1);
    Ks103_Write_byte(OldAdddr,0x02,0x92);
    delay_ms(1);
    Ks103_Write_byte(OldAdddr,0x02,0x9e);
    delay_ms(1);
    Ks103_Write_byte(OldAdddr,0x02,NewAddr);
    delay_ms(500);
}

//获取距离  mm   0xe8 0x02 0xb0/0xb4
//                        不带温度补偿 带温度补偿  0-5m
u32 Read_Ks103_Data(u8 SlaveAddress,u8 command)
{
	u32 distance=0;
	int count=800;
	Ks103_Write_byte(SlaveAddress,0x02,command);
	//delay_ms(1);                                        //安全延时  不带温度补偿探测
	delay_ms(90);                                       //带温度补偿探测
	while(--count||!SCL)                   //通过查询SCL线状态来智能识别探测是否结束
	{
		;
	}
	distance=Ks103_Read_byte(SlaveAddress,0x02);
  distance<<=8;
  distance+=Ks103_Read_byte(SlaveAddress,0x03);
	return distance;
}

float Get_Data(u32 data,u8 n)       //滤波计算距离
{
	u8 i;
	u32 sum=0;
	float value;
	for(i=0;i<n;i++) 
	{
		data=Read_Ks103_Data(SlaveAddress1,Notemp0_5m);             //不带温度补偿
		sum+=data;
	}
	value=sum/n;
	return value;
}


/*************************MODE接GND 串口模式*******************************/
void Send_Command(u8 command)
{
	USART2_SendByte(SlaveAddress1);   //发送默认器件地址
	delay_us(80);               //延时20-100us
	USART2_SendByte(0x02);         //发送寄存器2的地址
	delay_us(80);
	USART2_SendByte(command);
	delay_ms(90);                    //延时80ms 等待距离探测完后
}





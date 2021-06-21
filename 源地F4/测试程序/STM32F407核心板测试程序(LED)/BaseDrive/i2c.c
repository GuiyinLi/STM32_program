/******************** (C) COPYRIGHT  源地工作室 ********************************
 * 文件名  ：i2c.c
 * 描述    ：初始化IIC及一些基本的操作
 * 作者    ：zhuoyingxingyu
 * 淘宝    ：源地工作室http://vcc-gnd.taobao.com/
 * 论坛地址：极客园地-嵌入式开发论坛http://vcc-gnd.com/
 * 版本更新: 2015-02-20
 * 硬件连接: SCL--PB6,SDA--PB7 
 * 调试方式：J-Link-OB
******************************************************************************/	

//头文件
#include "i2c.h"
#include "stdio.h"
//#include "usmart.h"
//=============================================================================
//函数名称:I2C_Configuration
//功能概要:EEPROM管脚配置
//参数说明:无
//函数返回:无
//=============================================================================
void I2C_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
  /* Configure I2C2 pins: PB6->SCL and PB7->SDA */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
  GPIO_Init(GPIOB, &GPIO_InitStructure);//
  SDA_H;
	SCL_H;
}

//=============================================================================
//函数名称:I2C_delay
//功能概要:延迟时间
//参数说明:无
//函数返回:无
//=============================================================================
void I2C_delay(u16 nCount)
{	
   while(nCount--)
  {
  }
}

//=============================================================================
//函数名称:I2C_delay
//功能概要:延迟时间
//参数说明:无
//函数返回:无
//=============================================================================
void I2C_delayms(u16 nCount)
{	
	 while(nCount--)
  {
		I2C_delay(10000);
  }
   
}

//=============================================================================
//函数名称:I2C_Start
//功能概要:SCL高电平时,SDA有高电平变为低电平,产生IIC起始信号
//参数说明:无
//函数返回:无
//=============================================================================
 void I2C_Start(void)
{
	SDA_OUT();     //sda线输出,scl=H,SDA  1-->0;
	SDA_H;
	SCL_H;
	I2C_delay(70);//START:when CLK is high,DATA change form high to low 
	SDA_L;
	I2C_delay(70);
	SCL_L;
	I2C_delay(70);
}

//=============================================================================
//函数名称:I2C_Stop
//功能概要:SCL高电平时，SDA有低电平变为高电平时，为停止信号
//参数说明:无
//函数返回:无
//=============================================================================
 void I2C_Stop(void)
{
	SDA_OUT();//sda线输出
	SCL_L;
	I2C_delay(70);//STOP:when CLK is high DATA change form low to high
	SDA_L;
	I2C_delay(70);
	SCL_H;
	I2C_delay(70);
	SDA_H;
	I2C_delay(70);
}

//=============================================================================
//函数名称:I2C_Ack
//功能概要:产生ACK应答
//参数说明:无
//函数返回:无
//=============================================================================
void I2C_Ack(void)
{	
	SCL_L;
	SDA_OUT();
	I2C_delay(70);
	SDA_L;
	I2C_delay(70);
	SCL_H;
	I2C_delay(70);
	SCL_L;
	I2C_delay(70);
}

//=============================================================================
//函数名称:I2C_NoAck	    
//功能概要:不产生ACK应答		    
//参数说明:无
//函数返回:无
//=============================================================================
 void I2C_NoAck(void)
{	
	SCL_L;
	SDA_OUT();
	I2C_delay(70);
	SDA_H;
	I2C_delay(70);
	SCL_H;
	I2C_delay(70);
	SCL_L;
	I2C_delay(70);
}

//=============================================================================
//函数名称:I2C_WaitAck
//功能概要:等待应答信号到来
//参数说明:无
//函数返回:0，接收应答失败
//         1，接收应答成功
//=============================================================================
 uint8_t I2C_WaitAck(void) 	
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入 
	SDA_H;
	I2C_delay(70);
	SCL_H;
	I2C_delay(10);
	while(SDA_read)
	{
	//printf("%d",SDA_read);
		ucErrTime++;
		if(ucErrTime>200)
		{
			I2C_Stop();
			return 0;
		}
	}
	SCL_L;
	I2C_delay(70);
	return 1;
}

//=============================================================================
//函数名称:I2C_SendByte		    
//功能概要:主器件向从器件写一个字节的数据,数据从高位到低位		    
//参数说明:发送的数据
//函数返回:无
//=============================================================================
 void I2C_SendByte(uint8_t SendByte) 
{
	uint8_t i=8;
	SDA_OUT();
	SCL_L;
	while(i--)
	{
		SCL_L;
		I2C_delay(70);
		if(SendByte&0x80)
			 SDA_H;  
		else 
			 SDA_L;   
		SendByte<<=1;
		I2C_delay(70);
		SCL_H;
		I2C_delay(70);
	}
	SCL_L;
}

//=============================================================================
//函数名称:I2C_ReceiveByte	    
//功能概要:主器件从从器件读取一个字节的数据,数据从高位到低位	    
//参数说明:无
//函数返回:I2C总线返回的数据:receive
//=============================================================================	
uint8_t I2C_ReceiveByte(void)  
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;
	SDA_IN();  //SDA设置为输入
	SDA_H;	   			
	while(i--)
	{
		ReceiveByte<<=1;      
		SCL_L;
		I2C_delay(70);
		SCL_H;
		I2C_delay(70);	
		if(SDA_read)
		{
			ReceiveByte|=0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
}

//=============================================================================
//函数名称:I2C_WriteOneByte    
//功能概要:向指定地址写入一个数据	    
//参数说明:- DataToWrite: 待写入数据
//         - WriteAddr: 待写入地址
//函数返回:返回为:=1成功写入,=0失败
//=============================================================================	
uint8_t I2C_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{		
    I2C_Start();
    I2C_SendByte(0xA0); 
    I2C_WaitAck();
    I2C_SendByte((uint8_t)(WriteAddr & 0x00FF));   /* 设置低起始地址 */      
    I2C_WaitAck();	
    I2C_SendByte(DataToWrite);
    I2C_WaitAck();   
    I2C_Stop(); 
	  I2C_delayms(10);
	/* 注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)	*/
    /* Systick_Delay_1ms(10); */
    return 1;
}									  

//=============================================================================
//函数名称:I2C_ReadOneByte    
//功能概要:从指定地址读出一个数据    
//参数说明: ReadAddress: 读数的地址 
//函数返回:读到的数据
//=============================================================================	
uint8_t I2C_ReadOneByte(uint16_t ReadAddress)
{		
    uint8_t temp=0;
    I2C_Start();
    I2C_SendByte(0xA0); //发送写命令
    I2C_WaitAck();
    I2C_SendByte((uint8_t)(ReadAddress & 0x00FF));   /* 设置低起始地址 */      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(0xA1);//进入接收模式	
    I2C_WaitAck();
		temp=I2C_ReceiveByte();
    I2C_Stop();
    return temp;
}


//=============================================================================
//函数名称:I2C_WriteLenByte    
//功能概要:向指定地址开始写入指定个数的数据    
//参数说明:
//         WriteAddr :开始写入的地址 对24c02为0~255
//         pBuffer   :数据数组首地址
//         NumToWrite:要写入数据的个数
//函数返回:无
//=============================================================================	
void I2C_WriteLenByte(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
	uint16_t i;	
	for(i=WriteAddr;i<NumToWrite;i++)
	{
		I2C_WriteOneByte(i,pBuffer[i]);
	}
	//while(!I2C_WriteOneByte(i,pBuffer[i]));	
}									  

//=============================================================================
//函数名称:I2C_ReadLenByte    
//功能概要:读取一串数据	    
//参数说明: - ReadAddr: 待读出地址
//          - pBuffer: 存放读出数据的数组的首地址
//          - NumToRead:待读出长度
//函数返回:无
//=============================================================================	
void I2C_ReadLenByte(uint16_t ReadAddr, uint8_t *pBuffer,uint16_t NumToRead)
{	
	while(NumToRead)
	{
		*pBuffer++=I2C_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}

uint8_t I2C_Test(void)
{
		uint8_t WriteBuffer[256],ReadBuffer[256];
		uint16_t i;
	for(i=0; i<256; i++)
  {
    WriteBuffer[i]=i;	 
  }
	I2C_WriteLenByte(0, WriteBuffer, 256);
	for(i=0; i<1000; i++)
  {
      ;		
  }
		I2C_ReadLenByte(0, ReadBuffer, 256);
		if(  memcmp(WriteBuffer,ReadBuffer,sizeof(WriteBuffer)) == 0 ) /* 匹配数据 */
		return 1;
		else
		return 0;

}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/












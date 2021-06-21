#include "iic.h"
#include "delay.h"

void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;     //推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);               //拉高数据线和时钟线
}

void IIC_Start(void)
{
	SDA_OUT();
	SCL=1;
	SDA=1;
	delay_us(10);
	SDA=0;
	delay_us(10);
	SCL=0;
}

void IIC_Stop(void)
{
	SDA_OUT();
	SCL=0;
	SDA=0;
	delay_us(10);
	SCL=1;
	SDA=1;
	delay_us(10);
}

u8 IIC_Wait_Ack(void)         //0接收成功 1接受失败
{
	u8 t=0;
	SDA_IN();
	SDA=1;
	delay_us(6);
	SCL=1;
	delay_us(6);
	while(READ_SDA)
	{
		t++;
		if(t>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	SCL=0;
	return 0;
}

void IIC_Ack(void)
{
	SCL=0;
	SDA_OUT();
	SDA=0;
	delay_us(10);
	SCL=1;
	delay_us(10);
	SCL=0;
}

void IIC_NAck(void)
{
	SCL=0;
	SDA_OUT();
	SDA=1;
	delay_us(10);
	SCL=1;
	delay_us(10);
	SCL=0;
}
//发送一个字节
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	SCL=0;
	for(t=0;t<8;t++)
	{
		if((txd&0x80)<<7)
			SDA=1;
		else
			SDA=0;
		txd<<=1;
		delay_us(2);
		SCL=1;
		delay_us(2);
		SCL=0;
		delay_us(2);
	}
}
//读一个字节
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++)
	{
		SCL=0;
		delay_us(10);
		SCL=1;
		receive<<=1;
		if(READ_SDA) receive++;
		delay_us(5);
	}
	if(!ack) IIC_NAck();
	else IIC_Ack();
	return receive;
}



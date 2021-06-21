#include "led.h"
#include "stm32f10x.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //开始设置参数
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;            //第五个IO口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;    //IO口速度为50MHZ
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;    //推挽输出
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE,ENABLE);//使能GPIOB\E的时钟
	GPIO_Init(GPIOB,&GPIO_InitStructure);             //根据设置参数初始化GPIOB口
	GPIO_SetBits(GPIOB,GPIO_Pin_5);                   //将PB5置位高电平
	
	GPIO_Init(GPIOE,&GPIO_InitStructure);             //根据设置参数初始化GPIOE口
	GPIO_SetBits(GPIOE,GPIO_Pin_5);                   //将PE5置位高电平
	
}



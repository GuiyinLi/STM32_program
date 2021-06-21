#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	KEY_Init();     //按键端口初始化  就是初始化IO口为输入                             第一步
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);    //使能SYSCFG时钟
	/****************GPIOB4*************************************/
	/*****************设置IO口与中断线的映射关系************************/             //第三步
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource5);  //PA5--Line5
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);  //PA0--Line0
	
	/*****************配置EXTI 5****************/                                 //第四步
	EXTI_InitStructure.EXTI_Line=EXTI_Line5;    //中断线设置
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;    //使能
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;    //选择外部中断（还有事件中断）
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;   //对于上拉模式 采用下降沿触发 选择触发方式
	EXTI_Init(&EXTI_InitStructure);
	
	/***********************配置中断分组*****************************/
	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;          //选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;           //使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;  //设置抢占优先级为2 其值不能超过2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;        //设置响应优先级为2
	NVIC_Init(&NVIC_InitStructure);
	
	/*****************配置EXTI 5****************/                                 //第四步
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;    //中断线设置
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;    //使能
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;    //选择外部中断（还有事件中断）
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;   //对于上拉模式 采用下降沿触发 选择触发方式
	EXTI_Init(&EXTI_InitStructure);
	
	/***********************配置中断分组*****************************/
	NVIC_InitStructure.NVIC_IRQChannel=EXTI0_IRQn;          //选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;           //使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;  //设置抢占优先级为2 其值不能超过2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;        //设置响应优先级为2
	NVIC_Init(&NVIC_InitStructure);
	
}
	
/******************编写中断服务函数****************************/
void EXTI9_5_IRQHandler  (void)     //  外部中断5服务程序
{
	if(KEY0==0)
	{
		LED=!LED;
	}
	EXTI_ClearITPendingBit(EXTI_Line5);     //清楚中断标志位
}

void EXTI0_IRQHandler  (void)     //  外部中断5服务程序
{
	if(WK_UP==0)
	{
		LED=!LED;
	}
	EXTI_ClearITPendingBit(EXTI_Line0);     //清楚中断标志位
}




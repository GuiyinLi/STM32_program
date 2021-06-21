#ifndef __ADC_H
#define __ADC_H	 
#include "sys.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"

extern u16 adc_value;

void ADC1_init(void);
void ADC_init(void);
u16 Get_Adc(u8 ch);//获得ADC值  ch:通道值 0~3
u16 Get_Adc_Averge(u8 ch,u8 times);

		 				    
#endif

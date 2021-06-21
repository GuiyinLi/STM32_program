/***************STM32F103C8T6**********************
 * 文件名  ：main.c
 * 描述    : 舵机测试
 * 实验平台：STM32F103C8T6
 * 备注    ：使用定时器来产生pwm波，控制舵机动作
 * 接口    ：PB13接舵机信号

********************LIGEN*************************/
#include "stm32f10x.h"
#include "pwm_output.h"
#include "led.h"


int main(void)
{
  
	SystemInit(); //配置系统时钟为72M   
	
	TIM1_PWM_Init(); //TIM1 PWM波输出初始化，并使能TIM1 PWM输出
	
	Servo_out(); //舵机摆动
	
}





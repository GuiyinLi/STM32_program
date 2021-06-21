#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x_tim.h"
void TIM3_Int_Init(u16 arr,u16 psc);

extern volatile unsigned long long FreeRTOSRunTimeTicks;  //运行次数统计 实际时间=num*50us(50us为TIM3周期)
void ConfigureTimeForRunTimeStats(void);
#endif

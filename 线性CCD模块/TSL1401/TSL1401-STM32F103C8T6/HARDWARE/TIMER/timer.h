#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x_tim.h"
#include "sys.h"

#define TIM2_Init_ms(Period)	TIM2_Int_Init(Period*10-1,7199)  //ms
void TIM2_Int_Init(u16 arr,u16 psc);

#endif

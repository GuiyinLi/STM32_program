#include "control.h"
#include "timer.h"
#include "ccd.h"
u8 Flag_Target;

//周期为5ms的中断控制
void TIM2_IRQHandler(void) 
{
	if (TIM2->SR&0X0001) 
	{
		TIM2->SR&=~(1<<0); 
		Flag_Target=!Flag_Target; 
		if(Flag_Target)	
			RD_TSL();               
		else 
			getCCDMedian();			          
	}
}



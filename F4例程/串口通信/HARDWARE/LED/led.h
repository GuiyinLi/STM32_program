#ifndef __LED_H
#define __LED_H
#include "sys.h"


//LED端口定义
#define LED PGout(15)	// DS0 

void LED_Init(void);//初始化		 				    
#endif

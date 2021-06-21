#ifndef _led_h
#define _led_h
#include "sys.h"

#define LED0  PBout(5)    //PB5输出控制
#define LED1  PEout(5)    //PE5输出控制
void LED_Init(void);

#endif


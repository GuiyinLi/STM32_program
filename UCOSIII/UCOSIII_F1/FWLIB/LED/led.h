#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED0 PCout(13)
#define LED1 PBout(0)

void LED_Init(void);

#endif


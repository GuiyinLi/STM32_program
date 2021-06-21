#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

void LED_GPIO_Config(void);
void LED_Toggle(void);
void LED_On(void);
void LED_Off(void);
void Delayus(u16 i);
void Delayms(u16 t);
void LED_Twinkle(void);

#endif /* __LED_H */

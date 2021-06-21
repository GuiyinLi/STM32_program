#ifndef __DEBUG_H
#define __DEBUG_H	 
#include "sys.h"

#define DEBUG_MODE		 0
void usart1_send(u8 data);
char binToHex_high(u8 num);
char binToHex_low(u8 num);
void Landzo_CCD_Data(void);
void LPLD_CCD_Data(void);
void Multi_CCD_Data(void);
void Debug(void(*debug)(void));


#endif

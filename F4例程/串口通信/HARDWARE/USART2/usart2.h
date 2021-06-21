#ifndef _usart2_h
#define _usart2_h
#include "sys.h"

#define USART2_REC_LEN  200  	//定义最大接收字节数 200
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节
extern u16 USART2_RX_STA;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位
extern u16 len;
extern u8 flag;
void usart2_init(u32 bound);

#endif



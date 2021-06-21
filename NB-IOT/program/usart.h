/*
 * @Author: Ken Kaneki
 * @Date: 2021-05-25 23:51:20
 * @LastEditTime: 2021-05-25 23:52:36
 * @Description: README
 * @FilePath: \undefinedc:\Users\Dell\Desktop\usart.h
 */
#ifndef _usart3_h
#define _usart3_h
#include "sys.h"

#define USART3_MAX_RECV_LEN 1000 //最大接收缓存字节数
#define USART3_MAX_SEND_LEN 1000 //最大发送缓存字节数
#define USART3_RX_EN 1           //0,不接收;1,接收.

extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;                     //接收数据状态

void USART3_Init(u32 bound); //串口3初始化
void TIM3_Set(u8 sta);
void TIM3_Init(u16 arr, u16 psc);

#endif

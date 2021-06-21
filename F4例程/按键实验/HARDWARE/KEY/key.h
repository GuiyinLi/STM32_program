#ifndef _key_h
#define _key_h
#include "sys.h"
/******位操作*************/
//#define KEY0 PBin(4)
//#define WK_UP PAin(0)
/****************库函数******************/
#define KEY0 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)     //读取按键0
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)     //读取按键1

#define KEY0_PRES 1      //按键0按下，返回1
#define WK_UP_PRES 2     //按键1按下返回2

void KEY_Init(void);     //IO口初始化
u8 KEY_Scan(u8 mode);    //按键扫描函数

#endif



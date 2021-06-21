#ifndef _key_h
#define _key_h
#include "sys.h"
/******位操作*************/
#define KEY0 PBin(4)
#define KEY1 PBin(5)
//#define WK_UP PAin(0)
/****************库函数******************///左下方按键损坏
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)     //读取按键1

#define KEY0_PRES 1      //按键0按下，返回1
#define KEY1_PRES 3      //按键0按下，返回1
#define WK_UP_PRES 2     //按键1按下返回2

void KEY_Init(void);     //IO口初始化
u8 KEY_Scan(u8 mode);    //按键扫描函数

#endif



#ifndef _flag_h
#define _flag_h
#include "includes.h"
#include "sys.h"

#define KEY0_FLAG		0x01
#define KEY1_FLAG		0x02
#define KEYFLAGS_VALUE	0X00						
extern OS_FLAG_GRP	EventFlags;		//定义一个事件标志组
void flag_task_create(void); 

#endif


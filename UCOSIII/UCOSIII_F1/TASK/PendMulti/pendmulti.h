#ifndef _pendmulti_h
#define _pendmulti_h
#include "includes.h"
#include "sys.h"

extern OS_SEM	Test_Sem1;			//信号量1		
extern OS_Q	Test_Q;				//消息队列						
#define QUEUE_NUM		10	//消息队列长度
#define CORE_OBJ_NUM	2	//内核对象个数，一共2个：1个信号量和一个消息队列					

void pendmulti_task_creat(void);
#endif



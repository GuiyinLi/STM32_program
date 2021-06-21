#ifndef _message_queue_h
#define _message_queue_h
#include "includes.h"
#include "sys.h"

#define KEYMSG_Q_NUM	1	//按键消息队列的数量
#define DATAMSG_Q_NUM	10	//发送数据的消息队列的数量
#define TASK_Q_NUM	4		//发任务内建消息队列的长度

extern OS_Q KEY_Msg;				//定义一个消息队列，用于按键消息传递，模拟消息邮箱
extern OS_Q DATA_Msg;				//定义一个消息队列，用于发送数据

void message_queue_task_creat(void);
void check_msg_queue(OS_Q Msg); 
#endif


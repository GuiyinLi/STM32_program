#ifndef _queue_task_h
#define _queue_task_h
#include "start_task.h"
#include "queue.h"
 
#define MESSAGE_Q_NUM   10   	//发送数据的消息队列的数量 

extern QueueHandle_t Message_Queue;	//信息队列句柄
void check_msg_queue(QueueHandle_t Message_Queue);
#endif


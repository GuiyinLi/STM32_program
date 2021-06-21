#include "queue_task.h"
#include "led.h"
#include "key.h"
#include "stdlib.h"

/* ====================================== 队列相关API函数 ============================================== */
/*  函数名				       功能							参数    							      返回值                  备注        |
		xQueueSend()         后向入队         阻塞时间(portMAX_DELAY)     pdPASS/errQueueFull    任务级 发送  |
    xQueueSendToBack()	                     同上                                                         |
		xQueueSendToFront()  前向入队            同上                                                         |
		xQueueOverwrite()    后向入对           NULL                       带覆写功能 适用于一个字的传输      |
		------------------------------------------------------------------------------------------------------|
		xQueueSendFromISR()         后向入队   xHigherPriorityTaskWoken    pdTURE/errQueueFull  中断级 发送   |
    xQueueSendToBackFromISR()	                      同上     参数决定是否进行任务切换                     |
		xQueueSendToFrontFromISR()  前向入队            同上                                                  |
		xQueueOverwriteFromISR()    后向入对            同上               带覆写功能 适用于一个字的传输      |
		------------------------------------------------------------------------------------------------------|
		xQueueReceive()   读取后删除消息      阻塞时间                     pdTURE/pdFALSE      任务级 接收    |
		xQueuePeek()      读取后不删除消息                同上                                                |
		------------------------------------------------------------------------------------------------------|
		xQueueReceiveFromISR()   读取后删除消息     同下                      pdTURE/pdFALSE 中断级 接收      |
		xQueuePeekFromISR()      读取后不删除消息  xHigherPriorityTaskWoken   参数决定是否进行任务切换        |
    -------------------------------------  备注   --------------------------------------------------------|
		入队方式： 1.前向入队  2.后向入队  3.覆写入队
		ISR:需要通过xHigherPriorityTaskWoken来保存一个参数 该参数依据当前处理消息的优先级大小 
		                                                   决定是否进行任务切换
	  
*/
/* ===================================================================================================== */	
		        
		
	
QueueHandle_t Message_Queue;
TaskHandle_t QueueTask_Handler;

//查询Message_Queue队列中的总队列数量和剩余队列数量
void check_msg_queue(QueueHandle_t Message_Queue)
{
	  u8 msgq_remain_size;	//消息队列剩余大小
    u8 msgq_total_size;     //消息队列总大小
    
    taskENTER_CRITICAL();   //进入临界区
    msgq_remain_size=uxQueueSpacesAvailable(Message_Queue);//得到队列剩余大小
    msgq_total_size=uxQueueMessagesWaiting(Message_Queue)+msgq_remain_size;//得到队列总大小，总大小=使用+剩余的。
	  printf("Total Size:%d    Remain Size:%d\r\n",msgq_total_size,msgq_remain_size);
    taskEXIT_CRITICAL();    //退出临界区
}

void queue_task_creat(void)
{
	
	Message_Queue=xQueueCreate(MESSAGE_Q_NUM,USART_REC_LEN); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	xTaskCreate((TaskFunction_t )queue_task,             
                (const char*    )"queue_task",           
                (uint16_t       )QUEUE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )QUEUE_TASK_PRIO,        
                (TaskHandle_t*  )&QueueTask_Handler);  
}

void queue_task(void *pvParameters)
{
	char *rx_buffer;
	rx_buffer=(char *)malloc(sizeof(char)*USART_REC_LEN);
	while(1)
	{
		if(KEY_Scan(0)==KEY0_PRES)
		{
			if(xQueueReceive(Message_Queue,rx_buffer,portMAX_DELAY)==pdTRUE)   //请求消息Message_Queue 一直等待 接收到消息后立即从消息列表中删除
			{
				printf("queue_task receive message: %s\r\n",rx_buffer);
				free(rx_buffer);  //释放内存
				rx_buffer=(char *)malloc(sizeof(char)*USART_REC_LEN);
				LED0=!LED0;
				check_msg_queue(Message_Queue);
			}
	  }
	}
}



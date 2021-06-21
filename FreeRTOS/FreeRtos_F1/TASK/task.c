#include "start_task.h"
#include "led_task.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "list_task.h"
#include "key.h"
#include "api.h"
#include "queue_task.h"
#include "semaphore.h"
#include "timer_task.h"
#include "event_task.h"
#include "heap_task.h"
#include "tasknotify_task.h"
/* ====================== 任务操作相关的API函数 =================== */
/*
	vTaskSuspend()         任务挂起                                    |
	vTaskResume()          任务恢复                                    |
	vTaskResumeFromISR()   在中断里进行任务恢复                        |
	说明：在vTaskResumeFromISR()中 若返回值为pdTRUE                    |
	则需要portYIELD_FROM_ISR(pdTRUE)进行上下文切换 pdFALSE不需要       |
*/
/* ================================================================ */
TaskHandle_t StartTask_Handler;
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区->任务级   taskENTER_CRITICAL_From_ISR->中断级
	
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区      taskEXIT_CRITICAL_From_ISR->中断级
}

void start_task_creat()
{
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄  
		#if IS_OPEN_LED0_TASK
	  led0_task_creat();
	  #endif
								
    #if IS_OPEN_list_TASK
		list_task_creat();
		#endif
								
		#if IS_OPEN_api_TASK
		api_task_creat();
		#endif
								
		#if IS_OPEN_queue_TASK
		queue_task_creat();
		#endif
								
		#if IS_OPEN_semaphore_TASK
		SemaPhore_task_creat();
		#endif
		
		#if IS_OPEN_timers_TASK
		timercontrol_task_creat();
		#endif
		
		#if IS_OPEN_event_TASK
		event_task_creat();
		#endif
		
		#if IS_OPEN_heap_TASK
		heap_task_creat();
		#endif
		
		#if IS_OPEN_tasknotify_TASK
		TaskNotify_task_creat();
		#endif
}

void FreeRTOS_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4->16个抢占优先级 FreeRTOS无法处理亚优先级 抢占组4没有亚优先级	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  
	KEY_Init();
}

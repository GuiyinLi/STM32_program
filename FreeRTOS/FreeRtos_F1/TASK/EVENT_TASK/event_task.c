#include "event_task.h"
#include "led.h"
#include "key.h"

/* ====================================== 事件标志组相关API函数 ======================================== */
/*
		xEventGroupCreate()                    创建事件标志组                                                 |
    xEventGroupCreateStatic()              静态创建                                                       |       
 	  xEventGroupSetBits()	                 设定标志位为1                                                  |
    xEventGroupSetBitsFromISR()            设定标志位为1(中断级)	                                      	|
		xEventGroupClearBits()	               设定标志位为0                                                  |
    xEventGroupClearBitsFromISR()          设定标志位为0(中断级)	                                      	|
		xEventGroupWaitBits()                  等待指定的事件标志位                                           |
*/
/* ===================================================================================================== */

EventGroupHandle_t EventGroupHandler;
TaskHandle_t EventSetBit_Handler;
TaskHandle_t EventGroupTask_Handler;

//设置事件位的任务
void eventsetbit_task(void *pvParameters)
{
	u8 key;
	EventBits_t NewValue;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRES:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_0);
				  NewValue=xEventGroupGetBits(EventGroupHandler);	//获取事件组的值
				  printf("set eventbit eventvalue:%d\r\n",NewValue);
					break;
				case KEY1_PRES:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
				  NewValue=xEventGroupGetBits(EventGroupHandler);	//获取事件组的值
				  printf("event deal eventvalue:%d\r\n",NewValue);
					break;	
			}
		}
        vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
	EventBits_t EventValue;
	while(1)
	{

		if(EventGroupHandler!=NULL)
		{
			//等待事件组中的相应事件位
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,	  //事件标志组句柄	
										   (EventBits_t			)EVENTBIT_ALL,                        //事件标志位
										   (BaseType_t			)pdTRUE,				                      //时间位是否清零 pdTRUE->清零
										   (BaseType_t			)pdTRUE,                              //等待全部标志位还是任意标志位置1 pdTRUE->全部
								       (TickType_t			)portMAX_DELAY);	                    //阻塞时间 为portMAX_DELA则挂起任务
			printf("event deal eventvalue:%d\r\n",EventValue);
			LED1=!LED1;	
		
		}
	}
}

void event_task_creat(void)
{
	 //创建事件标志组
	EventGroupHandler=xEventGroupCreate();	 //创建事件标志组
	
	//创建设置事件位的任务
    xTaskCreate((TaskFunction_t )eventsetbit_task,             
                (const char*    )"eventsetbit_task",           
                (uint16_t       )EVENTSETBIT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTSETBIT_TASK_PRIO,        
                (TaskHandle_t*  )&EventSetBit_Handler);   	
    //创建事件标志组处理任务
    xTaskCreate((TaskFunction_t )eventgroup_task,             
                (const char*    )"eventgroup_task",           
                (uint16_t       )EVENTGROUP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
                (TaskHandle_t*  )&EventGroupTask_Handler);  
}

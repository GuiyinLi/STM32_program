#include "tasknotify_task.h"
#include "string.h"
#include "semaphore.h"
#include "led.h"
#include "key.h"
#include "event_task.h"

/* ====================================== 任务通知相关API函数 ========================================== */
/*  函数名				       功能							参数    							      返回值                  备注        |
    --------------------------------------发布通知--------------------------------------------------------|
		xTaskNotify()        发布通知   eAvtion(任务更新方法) pdFAIL/pdPASS  带通知值 不保留接收任务原通知值  |
    xTaskNotifyGive()	   发布通知    原通知值+1                          pdPASS               不保留      |
		xTaskNotifyAndQuery()发布通知                                      pdFAIL/pdPASS        保留原值      |
		以上函数均有对应的中断级函数
		---------------------------------------获取通知-------------------------------------------------------|
		ulTaskNotifyTake()   获取任务通知   pdTRUE(清零)/pdFALSE(减一)     操作之前的通知值                   |
		xTaskNotifyWait()    获取任务通知        4                          pdFALSE/pdTRUE                    |
		注：1.xTaskNotifyGive() ulTaskNotifyTake()常用于模拟二值和计数型信号量                                |
		    2.xTaskNotify() xTaskNotifyWait()常用于模拟消息邮箱和事件标志组                                   |
		3.用任务通知模拟的这些功能 效率高于原功能 但任务通知只能有一个接受任务 发布通知不会因为发送失败而阻塞 |
		4.configUSE_TASK_NOTIFICATIONS=1->开启任务通知
*/
/* ===================================================================================================== */	

TaskHandle_t TaskNotify_Binary_SemaPhore_Handler;   //模拟二值信号量

TaskHandle_t TaskNotify_SemapGiveTask_Handler;     //计数型信号量
TaskHandle_t TaskNotify_SemapTakeTask_Handler;

TaskHandle_t Task1Task_Handler;   //消息邮箱
TaskHandle_t Keyprocess_Handler;

TaskHandle_t TaskNotify_EventSetBit_Handler;  //事件标志组
TaskHandle_t TaskNotify_EventGroupTask_Handler;

void TaskNotify_Binary_SemaPhore_task(void *pvParameters)
{
	u8 len=0;
	u32 NotifyValue;
	while(1)
	{
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//获取任务通知 获取后将通知值清零
	  if(NotifyValue==1)									//清零之前的任务通知值为1，说明任务通知有效
		{
				len=USART_RX_STA&0x3fff;						//得到此次接收到的数据长度
				USART_RX_BUF[len]='\0';							//加上字符串结尾符号
				led_command=CommandProcess(USART_RX_BUF);		//命令解析
				if(led_command!=ERROR_COMMAND)
				{
					printf("命令为:%s\r\n",USART_RX_BUF);
					switch(led_command)						//处理命令
					{
						case LED0_ON: 
							LED0=0;
							break;
						case LED0_OFF:
							LED0=1;
							break;
						case LED1_ON:
							LED1=0;
							break;
						case LED1_OFF:
							LED1=1;
							break;
					}
				}
				else
				{
					printf("无效的命令，请重新输入!!\r\n");
				}
				USART_RX_STA=0;
				memset(USART_RX_BUF,0,USART_REC_LEN);			//串口接收缓冲区清零
		}
	}
}

//释放计数型信号量任务函数
void TaskNotify_SemapGive_task(void *pvParameters)
{
	u8 key;
	while(1)
	{
		key=KEY_Scan(0);           	//扫描按键
		if(TaskNotify_SemapTakeTask_Handler!=NULL)
		{
			if(key==KEY0_PRES)
			{  //参数为要发送的任务句柄  通知值+1
				xTaskNotifyGive(TaskNotify_SemapTakeTask_Handler);//发送任务通知 只能返回pdPASS 所有的发送通知函数无论发送失败还是成功 都不会阻塞
				LED0=!LED0;
			}
	  }
	}
}

//获取计数型信号量任务函数
void TaskNotify_SemapTake_task(void *pvParameters)
{
  u32 NotifyValue;
	while(1)
	{
		if(KEY_Scan(0)==KEY1_PRES)
		{
			NotifyValue=ulTaskNotifyTake(pdFALSE,portMAX_DELAY);//获取任务通知 获取后 在任务完成后将通知值-1
			printf("SemapTake_task: count_semaphore_value=%d\r\n",NotifyValue-1);
		}
	}
}

//task1任务函数
void task1_task(void *pvParameters)
{
	u8 key;
  BaseType_t err;
	while(1)
	{
		key=KEY_Scan(0);            			//扫描按键
    if((Keyprocess_Handler!=NULL)&&(key))   
    {
			err=xTaskNotify((TaskHandle_t	)Keyprocess_Handler,		//接收任务通知的任务句柄
							(uint32_t		)key,						//任务通知值
							(eNotifyAction	)eSetValueWithoutOverwrite);	//覆写的方式发送任务通知 模拟消息邮箱时  参数为eSetValueWithoutOverwrite/eSetValueWithOverwrite
			if(err==pdFAIL)
			{
				printf("任务通知发送失败\r\n");
			}
    }
    vTaskDelay(10);          
	}
}


//Keyprocess_task函数
void Keyprocess_task(void *pvParameters)
{
	uint32_t NotifyValue;
	BaseType_t err;
	
	while(1)
	{
		err=xTaskNotifyWait((uint32_t	)0x00,				//进入函数的时候不清除任务bit
							(uint32_t	)ULONG_MAX,			//退出函数的时候清除所有的bit 0xffffffff
							(uint32_t*	)&NotifyValue,		//保存任务通知值
							(TickType_t	)portMAX_DELAY);	//阻塞时间
		if(err==pdTRUE)				//获取任务通知成功
		{
			switch((u8)NotifyValue)
			{
				case KEY1_PRES:	
             LED1=!LED1;
					   break;
				case KEY0_PRES:		
             LED0=!LED0;
             break;
			}
		}
	}
}

//设置事件位的任务
void TaskNotify_eventsetbit_task(void *pvParameters)
{
	u8 key;

	while(1)
	{
		
		if(TaskNotify_EventGroupTask_Handler!=NULL)
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRES:
					xTaskNotify((TaskHandle_t	)TaskNotify_EventGroupTask_Handler,//接收任务通知的任务句柄
								(uint32_t		)EVENTBIT_0,			//要更新的bit
								(eNotifyAction	)eSetBits);				//更新指定的bit
					break;
				case KEY1_PRES:
					xTaskNotify((TaskHandle_t	)TaskNotify_EventGroupTask_Handler,//接收任务通知的任务句柄
								(uint32_t		)EVENTBIT_1,			//要更新的bit
								(eNotifyAction	)eSetBits);				//更新指定的bit
					break;	
			}
		}
        vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//事件标志组处理任务
void TaskNotify_eventgroup_task(void *pvParameters)
{
	u8 enevtvalue;
	static u8 event0flag,event1flag;
	uint32_t NotifyValue;
	BaseType_t err;
	while(1)
	{
	//获取任务通知值
		err=xTaskNotifyWait((uint32_t	)0x00,				//进入函数的时候不清除任务bit
							(uint32_t	)ULONG_MAX,			//退出函数的时候清除所有的bit
							(uint32_t*	)&NotifyValue,		//保存任务通知值
							(TickType_t	)portMAX_DELAY);	//阻塞时间
		
		if(err==pdPASS)	   //任务通知获取成功
		{
			if((NotifyValue&EVENTBIT_0)!=0)			//事件0发生	
			{
				event0flag=1;	
			}				
			else if((NotifyValue&EVENTBIT_1)!=0)	//事件1发生	
			{
				event1flag=1;
			}
			enevtvalue=event0flag|(event1flag<<1);	//模拟事件标志组值
			printf("任务通知值为:%d\r\n",enevtvalue);
			
			if((event0flag==1)&&(event1flag==1))	//三个事件都同时发生
			{
				LED1=!LED1;	
				event0flag=0;								//标志清零
				event1flag=0;
				enevtvalue=event0flag|(event1flag<<1);	//模拟事件标志组值
				printf("任务通知值为:%d\r\n",enevtvalue);
			}
		}
		else
		{
			printf("任务通知获取失败\r\n");
		}
	}
}

void TaskNotify_task_creat(void)
{
	#if TASKNOTIFY_BINARY_SEMAPHORE
	xTaskCreate((TaskFunction_t )TaskNotify_Binary_SemaPhore_task,     
                (const char*    )"TaskNotify_Binary_SemaPhore_task",   
                (uint16_t       )TASKNOTIFY_BINARY_SEMAPHORE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASKNOTIFY_BINARY_SEMAPHORE_TASK_PRIO,
                (TaskHandle_t*  )&TaskNotify_Binary_SemaPhore_Handler); 
	#endif
								
	#if TASKNOTIFY_COUNT_SEMAPHORE

		 //创建释放信号量任务
    xTaskCreate((TaskFunction_t )TaskNotify_SemapGive_task,             
                (const char*    )"tasknotify_semapgive_task",           
                (uint16_t       )TASKNOTIFY_SEMAPGIVE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASKNOTIFY_SEMAPGIVE_TASK_PRIO,        
                (TaskHandle_t*  )&TaskNotify_SemapGiveTask_Handler);   
    //创建获取信号量任务
    xTaskCreate((TaskFunction_t )TaskNotify_SemapTake_task,     
                (const char*    )"semaptake_task",   
                (uint16_t       )TASKNOTIFY_SEMAPTAKE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASKNOTIFY_SEMAPTAKE_TASK_PRIO,
                (TaskHandle_t*  )&TaskNotify_SemapTakeTask_Handler); 
		#endif
		
		#if TASKNOTIFY_FIFO
		 //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建按键处理任务
    xTaskCreate((TaskFunction_t )Keyprocess_task,     
                (const char*    )"keyprocess_task",   
                (uint16_t       )KEYPROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )KEYPROCESS_TASK_PRIO,
                (TaskHandle_t*  )&Keyprocess_Handler); 
		#endif
								
		#if TASKNOTIFY_EVENT
		//创建设置事件位的任务
    xTaskCreate((TaskFunction_t )TaskNotify_eventsetbit_task,             
                (const char*    )"TaskNotify_eventsetbit_task",           
                (uint16_t       )TASKNOTIFY_EVENTSETBIT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASKNOTIFY_EVENTSETBIT_TASK_PRIO,        
                (TaskHandle_t*  )&TaskNotify_EventSetBit_Handler);   	
    //创建事件标志组处理任务
    xTaskCreate((TaskFunction_t )TaskNotify_eventgroup_task,             
                (const char*    )"TaskNotify_eventgroup_task",           
                (uint16_t       )TASKNOTIFY_EVENTGROUP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASKNOTIFY_EVENTGROUP_TASK_PRIO,        
                (TaskHandle_t*  )&TaskNotify_EventGroupTask_Handler);    
		#endif
}



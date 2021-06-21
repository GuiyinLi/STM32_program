#include "semaphore.h"
#include "led.h"
#include "usart.h"
#include "string.h"
#include "key.h"
#include "delay.h"
/* ====================================== 信号量相关API函数 ============================================== */
/*  函数名				       功能							参数    							      返回值                  备注        |
    --------------------------------------二值信号量------------------------------------------------------|
		xSemaphoreCreateBinary()          动态创建二值信号量(默认无效)   创建成功的二值信号量句柄             |
    xSemaphoreCreateBinaryStatic()	  静态创建二值信号量                                                  |
		xSemaphoreGive()                  释放信号量          pdPASS/errQueueFull 除递归互斥信号量外  任务级  |
		xSemaphoreGiveFromISR()           仅释放二值和计数型信号量      同上           任务切换  中断级       |
		xSemaphoreTake()          获取二值 计数 互斥信号量  阻塞时间    pdTURE/pdFALSE      任务级            |
    xSemaphoreCreateBinaryFromISR()	 获取二值 计数 信号量           pdPASS/pdFALSE  中断级 任务切换       |
		---------------------------------------计数信号量-----------------------------------------------------|
		xSemaphoreCreateCounting()        创建计数型信号量(初始值)     创建成功的计数型信号量句柄             |
		xSemaphoreCreateCountingStatic()              同上                                                    |
		注：用于事件计数(初始值=0)和资源管理(初始值>0)                                                        |
		---------------------------------------互斥信号量-----------------------------------------------------|
		xSemaphoreCreateMutex()        创建互斥信号量(默认有效)  创建成功的互斥信号量句柄                     |
		xSemaphoreCreateMutexStatic()   静态创建                                                              |
		注：防止优先级翻转的现象                                                                              |
				不能用在ISR中                                                                                     |
        SemaphoreHandle_t MutexSemaphore;	//互斥信号量                                                    |
        MutexSemaphore=xSemaphoreCreateMutex();			                                                     	|
				对于(递归)互斥信号量 在一个任务中获取后还要释放该信号量 即是用完之后一定要释放                    |			                                                     	|
    ---------------------------------------递归互斥信号量-------------------------------------------------|
		xSemaphoreCreateRecursiveMutex() 动态创建递归互斥信号量(默认有效)                                     |
		xSemaphoreCreateRecursiveMutexStatic() 静态创建                                                       |
		xSemaphoreTakeRecursive()              获取递归互斥信号量                                             |
		xSemaphoreGiveRecursive()              释放递归互斥信号量                                             |
		注：1.(递归)互斥信号量不能用在ISR中                                                                   |
		    2.(递归)互斥信号量一定要在同一个任务中获取和释放(先获取后释放)                                    |
				3.递归互斥信号量获取次数无限 只有在同一个任务中释放同样的次数 才可被其他任务获取                  |
				  虽然每次释放都会成功 但只有在最后一次释放 才是真正的调用信号量释放函数 其他情况下都是           |
					获取计数量减一                                                                                  |
*/
/* ===================================================================================================== */	
LED_COMMAND led_command=ERROR_COMMAND;
SemaphoreHandle_t Binary_Semaphore;	    //二值信号量句柄
TaskHandle_t Binary_SemaPhore_Handler;  //任务句柄

SemaphoreHandle_t Count_Semaphore;  //计数型信号量句柄
TaskHandle_t SemapGiveTask_Handler;
TaskHandle_t SemapTakeTask_Handler;

SemaphoreHandle_t Recursive_Semaphore;  //递归互斥信号量
TaskHandle_t Recursive_SemaPhore_Handler;
 
//命令处理函数，将字符串命令转换成命令值
//str：命令
//返回值: 0XFF，命令错误；其他值，命令值
LED_COMMAND CommandProcess(u8 *str)
{
	LED_COMMAND led_command=ERROR_COMMAND;
	if(strcmp((char*)str,"LED0 ON")==0) led_command=LED0_ON;
	else if(strcmp((char*)str,"LED0 OFF")==0) led_command=LED0_OFF;
	else if(strcmp((char*)str,"LED1 ON")==0) led_command=LED1_ON;
	else if(strcmp((char*)str,"LED1 OFF")==0) led_command=LED1_OFF;
	else led_command=ERROR_COMMAND;
	return led_command;
}

void Binary_SemaPhore_task(void *pvParameters)
{
	u8 len=0;
	BaseType_t err=pdFALSE;   //存储获取信号量时的返回值
	while(1)
	{
		if(Binary_Semaphore!=NULL)
		{
			err=xSemaphoreTake(Binary_Semaphore,portMAX_DELAY);	//获取信号量
			if(err==pdTRUE)										//获取信号量成功
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
}

//释放计数型信号量任务函数
void SemapGive_task(void *pvParameters)
{
	u8 key;
    u8 semavalue;
	BaseType_t err;
	while(1)
	{
		key=KEY_Scan(0);           	//扫描按键
		if(Count_Semaphore!=NULL)  	//计数型信号量创建成功
		{
		  if(key==KEY0_PRES)
			{
				err=xSemaphoreGive(Count_Semaphore);//释放计数型信号量
				if(err==pdFALSE)
				{
					printf("信号量释放失败!!!\r\n");
				}
				semavalue=uxSemaphoreGetCount(Count_Semaphore);	//获取计数型信号量值
				printf("SemapGive_task: count_semaphore_value=%d\r\n",semavalue);
			}
		}
	}
}

//获取计数型信号量任务函数
void SemapTake_task(void *pvParameters)
{
  u8 num;
  u8 semavalue;
	while(1)
	{
		if(KEY_Scan(0)==KEY1_PRES)
		{
			xSemaphoreTake(Count_Semaphore,portMAX_DELAY); 	//等待数值信号量
			num++;
			semavalue=uxSemaphoreGetCount(Count_Semaphore); 	//获取数值信号量值
			printf("SemapTake_task: count_semaphore_value=%d\r\n",semavalue);
		}
	}
}

void Recursive_SemaPhore_task(void *pvParameters)
{
	BaseType_t err;
	u8 take_nums=0;
	while(1)
	{
		if(KEY_Scan(0)==KEY0_PRES)
		{
			err=xSemaphoreTakeRecursive(Recursive_Semaphore,portMAX_DELAY);
			if(err==pdPASS)
			{
				take_nums++;
				printf("take_nums :  %d\r\n",take_nums);
			}
			else
			{
				printf("Take Failed\r\n");
			}
		}
		else if(KEY_Scan(0)==KEY1_PRES)
		{
			err=xSemaphoreGiveRecursive(Recursive_Semaphore);
			if(err==pdPASS)
			{
				take_nums--;
				printf("take_nums :  %d\r\n",take_nums);  //只有最后一次释放才是真正的释放成功 其他任务才可获取该信号量
			}
			else
			{
				printf("This Task not Take Recursive_Semaphore\r\n");
			}
		}
	}
}
void SemaPhore_task_creat(void)
{
    #if IS_OPEN_binary_semaphore_TASK
	  //创建二值信号量
	  Binary_Semaphore=xSemaphoreCreateBinary();	
    xTaskCreate((TaskFunction_t )Binary_SemaPhore_task,     
                (const char*    )"Binary_SemaPhore_task",   
                (uint16_t       )BINARY_SEMAPHORE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )BINARY_SEMAPHORE_TASK_PRIO,
                (TaskHandle_t*  )&Binary_SemaPhore_Handler); 
		#endif
		#if IS_OPEN_count_semaphore_TASK
		//创建计数型信号量
	  Count_Semaphore=xSemaphoreCreateCounting(255,0);	//最大计数值 初始值
		 //创建释放信号量任务
    xTaskCreate((TaskFunction_t )SemapGive_task,             
                (const char*    )"semapgive_task",           
                (uint16_t       )SEMAPGIVE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )SEMAPGIVE_TASK_PRIO,        
                (TaskHandle_t*  )&SemapGiveTask_Handler);   
    //创建获取信号量任务
    xTaskCreate((TaskFunction_t )SemapTake_task,     
                (const char*    )"semaptake_task",   
                (uint16_t       )SEMAPTAKE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SEMAPTAKE_TASK_PRIO,
                (TaskHandle_t*  )&SemapTakeTask_Handler); 
		#endif
		
		#if IS_OPEN_recursive_semaphore_TASK
		Recursive_Semaphore=xSemaphoreCreateRecursiveMutex();
		xTaskCreate((TaskFunction_t )Recursive_SemaPhore_task,     
                (const char*    )"Recursive_SemaPhore_task",   
                (uint16_t       )RECURSIVE_SEMAPHORE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )RECURSIVE_SEMAPHORE_TASK_PRIO,
                (TaskHandle_t*  )&Recursive_SemaPhore_Handler);
		#endif
}


#include "api.h"
#include "key.h"
#include "delay.h"
#include "string.h"
#include "timer.h"

/* ==================== 统计任务运行时间以及所占百分比 =================== */
/*
	1.用于计算的时钟精度要高于系统时钟10-20倍(HZ)->时基
	2.实际运行时间=运行次数(FreeRTOSRunTimeTicks)*50us(TIM周期)
	3.宏定义 FreeRTOS与运行时间和任务状态收集有关的配置选项(3个开关宏定义 1个全局变量 1个函数)
	4.在调试时使用 平常使用非常耗时
*/
/* ======================================================================= */

//                           其他
/*
	任务运行状态：B：阻塞态  R：运行态/就绪态  S：挂起态  D：删除态
*/
char InfoBuffer[1000];	
TaskHandle_t ApiTask_Handler;

void api_task(void *pvParameters)
{
	#if DEBUG
	u32 TotalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	//第二步：函数vTaskGetInfo()的使用
	TaskHandle_t TaskHandle;  //指针	 
	TaskStatus_t TaskStatus; //结构体
	
	//第三步：函数eTaskGetState()的使用	
	eTaskState TaskState;
	char TaskInfo[10];
	
	//第五步：函数vTaskGetRunTimeStats()的使用
	char RunTimeInfo[400];		//保存任务运行时间信息
	
	//第一步：函数uxTaskGetSystemState()的使用
	printf("/********第一步：函数uxTaskGetSystemState()的使用**********/\r\n");
	ArraySize=uxTaskGetNumberOfTasks();		//获取系统任务数量
	StatusArray=pvPortMalloc(ArraySize*sizeof(TaskStatus_t));//申请内存
	if(StatusArray!=NULL)					//内存申请成功
	{
		ArraySize=uxTaskGetSystemState((TaskStatus_t* 	)StatusArray, 	//任务信息存储数组
									   (UBaseType_t		)ArraySize, 	//任务信息存储数组大小
								       (uint32_t*		)&TotalRunTime);//保存系统总的运行时间
		printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
		for(x=0;x<ArraySize;x++)
		{
			//通过串口打印出获取到的系统任务的有关信息，比如任务名称、
			//任务优先级和任务编号。
			printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",				
					StatusArray[x].pcTaskName,
					(int)StatusArray[x].uxCurrentPriority,
					(int)StatusArray[x].xTaskNumber);
			
		}
	}
	vPortFree(StatusArray);	//释放内存
	printf("/**************************结束***************************/\r\n");
	printf("按下KEY0键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY0_PRES) delay_ms(10);		//等待KEY_UP键按下
	

	
	printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
	TaskHandle=xTaskGetHandle("api_task");			//根据任务名获取任务句柄。
	//获取LED0_Task的任务信息
	vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//任务句柄
				 (TaskStatus_t*	)&TaskStatus, 		//任务信息结构体
				 (BaseType_t	)pdTRUE,			//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);			//函数自己获取任务运行壮态
	//通过串口打印出指定任务的有关信息。
	printf("任务名:                %s\r\n",TaskStatus.pcTaskName);
	printf("任务编号:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("任务状态:              %d\r\n",TaskStatus.eCurrentState);
	printf("任务当前优先级:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("任务基优先级:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("任务堆栈基地址:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("任务堆栈历史剩余最小值:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************结束***************************/\r\n");
	printf("按下KEY0键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY0_PRES) delay_ms(10);		//等待KEY_UP键按下
	
	
	printf("/***********第三步：函数eTaskGetState()的使用*************/\r\n");
	TaskHandle=xTaskGetHandle("api_task");		//根据任务名获取任务句柄。
	TaskState=eTaskGetState(TaskHandle);			//获取query_task任务的任务壮态
	memset(TaskInfo,0,10);						//清空内存
	switch((int)TaskState)
	{
		case 0:
			sprintf(TaskInfo,"Running");
			break;
		case 1:
			sprintf(TaskInfo,"Ready");
			break;
		case 2:
			sprintf(TaskInfo,"Suspend");
			break;
		case 3:
			sprintf(TaskInfo,"Delete");
			break;
		case 4:
			sprintf(TaskInfo,"Invalid");
			break;
	}
	printf("任务状态值:%d,对应的状态为:%s\r\n",TaskState,TaskInfo);
	printf("/**************************结束**************************/\r\n");
	printf("按下KEY0键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY0_PRES) delay_ms(10);		//等待KEY_UP键按下
	
	//第四步：函数vTaskList()的使用	
	printf("/*************第四步：函数vTaskList()的使用*************/\r\n");
	vTaskList(InfoBuffer);							//获取所有任务的信息
	printf("%s\r\n",InfoBuffer);					//通过串口打印所有任务的信息
	printf("/**************************结束**************************/\r\n");
	while(KEY_Scan(0)!=KEY0_PRES) delay_ms(10);		//等待KEY_UP键按下
	
	printf("/*************第五步：函数vTaskGetRunTimeStats()的使用*************/\r\n");
	memset(RunTimeInfo,0,400);				//信息缓冲区清零
	vTaskGetRunTimeStats(RunTimeInfo);		//获取任务运行时间信息
	printf("任务名\t\t运行时间\t运行所占百分比\r\n");
	printf("%s\r\n",RunTimeInfo);   //实际运行时间=次数*50us
	printf("/**************************结束**************************/\r\n");
	#endif
}

void api_task_creat(void)
{
	 xTaskCreate((TaskFunction_t )api_task,     
                (const char*    )"api_task",   
                (uint16_t       )API_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )API_TASK_PRIO,
                (TaskHandle_t*  )&ApiTask_Handler); 
}



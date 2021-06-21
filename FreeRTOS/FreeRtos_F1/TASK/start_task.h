#ifndef _start_task_h
#define _start_task_h
#include "FreeRTOS.h"
#include "task.h"


//0是最低优先级 用于空闲任务的运行 数字越小 优先级越低

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
extern TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);
void start_task_creat(void);
	
//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		128
//任务句柄
extern TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);
void led0_task_creat(void);

//任务优先级
#define LIST_TASK_PRIO		3
//任务堆栈大小	
#define LIST_STK_SIZE 		128  
//任务句柄
extern TaskHandle_t ListTask_Handler;
//任务函数
void list_task(void *pvParameters);
void list_task_creat(void);


//任务优先级
#define API_TASK_PRIO		3
//任务堆栈大小	
#define API_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t ApiTask_Handler;
//任务函数
void api_task(void *pvParameters);
void api_task_creat(void);


//任务优先级
#define QUEUE_TASK_PRIO		4
//任务堆栈大小	
#define QUEUE_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t QueueTask_Handler;
//任务函数
void queue_task(void *pvParameters);
void queue_task_creat(void);

//任务优先级
#define BINARY_SEMAPHORE_TASK_PRIO 5
//任务堆栈大小	
#define BINARY_SEMAPHORE_STK_SIZE  256 
//任务句柄
extern TaskHandle_t Binary_SemaPhore_Handler;
//任务函数
void Binary_SemaPhore_task(void *pvParameters);
void SemaPhore_task_creat(void);

//任务优先级
#define SEMAPGIVE_TASK_PRIO		6
//任务堆栈大小	
#define SEMAPGIVE_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t SemapGiveTask_Handler;
//任务函数
void SemapGive_task(void *pvParameters);

//任务优先级
#define SEMAPTAKE_TASK_PRIO 	7
//任务堆栈大小	
#define SEMAPTAKE_STK_SIZE  	256 
//任务句柄
extern TaskHandle_t SemapTakeTask_Handler;
//任务函数
void SemapTake_task(void *pvParameters);

//任务优先级
#define RECURSIVE_SEMAPHORE_TASK_PRIO 8
//任务堆栈大小	
#define RECURSIVE_SEMAPHORE_STK_SIZE  256 
//任务句柄
extern TaskHandle_t Recursive_SemaPhore_Handler;
//任务函数
void Recursive_SemaPhore_task(void *pvParameters);

//任务优先级
#define TIMERCONTROL_TASK_PRIO	9
//任务堆栈大小	
#define TIMERCONTROL_STK_SIZE 	256  
//任务句柄
extern TaskHandle_t TimerControlTask_Handler;
//任务函数
void timercontrol_task(void *pvParameters);
void timercontrol_task_creat(void);


//任务优先级
#define EVENTSETBIT_TASK_PRIO	10
//任务堆栈大小	
#define EVENTSETBIT_STK_SIZE 	256  
//任务句柄
extern TaskHandle_t EventSetBit_Handler;
//任务函数
void eventsetbit_task(void *pvParameters);

//任务优先级
#define EVENTGROUP_TASK_PRIO	11
//任务堆栈大小	
#define EVENTGROUP_STK_SIZE 	256  
//任务句柄
extern TaskHandle_t EventGroupTask_Handler;
//任务函数
void eventgroup_task(void *pvParameters);
void event_task_creat(void);

//任务优先级
#define HEAP_TASK_PRIO	12
//任务堆栈大小	
#define HEAP_STK_SIZE 	128
//任务句柄
extern TaskHandle_t HeapTask_Handler;
//任务函数
void heap_task(void *p_arg);
void heap_task_creat(void);

//任务优先级
#define TASKNOTIFY_BINARY_SEMAPHORE_TASK_PRIO 13
//任务堆栈大小	
#define TASKNOTIFY_BINARY_SEMAPHORE_STK_SIZE  256 
//任务句柄
extern TaskHandle_t TaskNotify_Binary_SemaPhore_Handler;
//任务函数
void TaskNotify_Binary_SemaPhore_task(void *pvParameters);

//任务优先级
#define TASKNOTIFY_SEMAPGIVE_TASK_PRIO		14
//任务堆栈大小	
#define TASKNOTIFY_SEMAPGIVE_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t TaskNotify_SemapGiveTask_Handler;
//任务函数
void TaskNotify_SemapGive_task(void *pvParameters);

//任务优先级
#define TASKNOTIFY_SEMAPTAKE_TASK_PRIO 	15
//任务堆栈大小	
#define TASKNOTIFY_SEMAPTAKE_STK_SIZE  	256 
//任务句柄
extern TaskHandle_t TaskNotify_SemapTakeTask_Handler;
//任务函数
void TaskNotify_SemapTake_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		16
//任务堆栈大小	
#define TASK1_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define KEYPROCESS_TASK_PRIO 17
//任务堆栈大小	
#define KEYPROCESS_STK_SIZE  256 
//任务句柄
extern TaskHandle_t Keyprocess_Handler;
//任务函数
void Keyprocess_task(void *pvParameters);

//任务优先级
#define TASKNOTIFY_EVENTSETBIT_TASK_PRIO	18
//任务堆栈大小	
#define TASKNOTIFY_EVENTSETBIT_STK_SIZE 	256  
//任务句柄
extern TaskHandle_t TaskNotify_EventSetBit_Handler;
//任务函数
void TaskNotify_eventsetbit_task(void *pvParameters);

//任务优先级
#define TASKNOTIFY_EVENTGROUP_TASK_PRIO	19
//任务堆栈大小	
#define TASKNOTIFY_EVENTGROUP_STK_SIZE 	256  
//任务句柄
extern TaskHandle_t TaskNotify_EventGroupTask_Handler;
//任务函数
void TaskNotify_eventgroup_task(void *pvParameters);

void TaskNotify_task_creat(void);

#define IS_OPEN_LED0_TASK       0
#define IS_OPEN_list_TASK       0
#define IS_OPEN_api_TASK        0
#define IS_OPEN_queue_TASK      0
#define IS_OPEN_semaphore_TASK  0
#define IS_OPEN_timers_TASK     0
#define IS_OPEN_event_TASK      0
#define IS_OPEN_heap_TASK       0
#define IS_OPEN_tasknotify_TASK 0
void FreeRTOS_Init(void);
#endif






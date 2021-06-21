#ifndef _task_h
#define _task_h
#include "includes.h"
#include "sys.h"

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

//任务优先级    
#define START_TASK_PRIO		3
//任务堆栈大小  
#define START_STK_SIZE 		512
//任务控制块
extern OS_TCB StartTaskTCB;
//任务堆栈	
extern CPU_STK START_TASK_STK[START_STK_SIZE];  //堆栈大小=START_STK_SIZE*4字节
//任务函数
void start_task(void *p_arg);
void start_Task_create(void);

//任务优先级
#define LED_TASK_PRIO		4
//任务堆栈大小	
#define LED_STK_SIZE 		128
//任务控制块
extern OS_TCB LedTaskTCB;
//任务堆栈	
extern CPU_STK LED_TASK_STK[LED_STK_SIZE];
void led_task(void *p_arg);
void led_task_create(void);

#define IS_OPEN_LED_TASK 1

void BSP_Init(void);

#endif



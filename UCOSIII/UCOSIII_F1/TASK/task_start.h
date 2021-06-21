#ifndef _task_start_h_
#define _task_start_h_
#include "includes.h"
#include "sys.h"
extern float float_num;
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
#define LED0_TASK_PRIO		4
//任务堆栈大小	
#define LED0_STK_SIZE 		128
//任务控制块
extern OS_TCB Led0TaskTCB;
//任务堆栈	
extern CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);
void led0_task_create(void);

//任务优先级
#define LED1_TASK_PRIO		5
//任务堆栈大小	
#define LED1_STK_SIZE 		128  //*4个字节
//任务控制块
extern OS_TCB Led1TaskTCB;
//任务堆栈	
extern CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *p_arg);
void led1_task_create(void);

//任务优先级
#define FLOAT_TASK_PRIO		6
//任务堆栈大小
#define FLOAT_STK_SIZE		128
//任务控制块
extern OS_TCB	FloatTaskTCB;
//任务堆栈
extern __align(8) CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
//任务函数
void float_task(void *p_arg);
void float_test_task_create(void);

//任务优先级
#define HIGH_TASK_PRIO			7
//任务堆栈大小	
#define HIGH_STK_SIZE 			128
//任务控制块
extern OS_TCB High_TaskTCB;
//任务堆栈	
extern CPU_STK HIGH_TASK_STK[HIGH_STK_SIZE];
void high_task(void *p_arg);


//任务优先级
#define MIDDLE_TASK_PRIO		8
//任务堆栈大小	
#define MIDDLE_STK_SIZE 		128
//任务控制块
extern OS_TCB Middle_TaskTCB;
//任务堆栈	
extern CPU_STK MIDDLE_TASK_STK[MIDDLE_STK_SIZE];
void middle_task(void *p_arg);

//任务优先级
#define LOW_TASK_PRIO			9
//任务堆栈大小	
#define LOW_STK_SIZE 			128
//任务控制块
extern OS_TCB Low_TaskTCB;
//任务堆栈	
extern CPU_STK LOW_TASK_STK[LOW_STK_SIZE];
void low_task(void *p_arg);

//任务优先级
#define MAIN_TASK_PRIO		10
//任务堆栈大小	
#define MAIN_STK_SIZE 		128
//任务控制块
extern OS_TCB Main_TaskTCB;
//任务堆栈	
extern CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);

//任务优先级
#define KEYPROCESS_TASK_PRIO 	11
//任务堆栈大小	
#define KEYPROCESS_STK_SIZE 	128
//任务控制块
extern OS_TCB Keyprocess_TaskTCB;
//任务堆栈	
extern CPU_STK KEYPROCESS_TASK_STK[KEYPROCESS_STK_SIZE];
//任务函数
void Keyprocess_task(void *p_arg);

//任务优先级
#define FLAG_MAIN_TASK_PRIO		12
//任务堆栈大小	
#define FLAG_MAIN_STK_SIZE 		128
//任务控制块
extern OS_TCB FLAG_Main_TaskTCB;
//任务堆栈	
extern CPU_STK FLAG_MAIN_TASK_STK[MAIN_STK_SIZE];
void flag_main_task(void *p_arg);

//任务优先级
#define FLAGSPROCESS_TASK_PRIO	13
//任务堆栈大小	
#define FLAGSPROCESS_STK_SIZE 	128
//任务控制块
extern OS_TCB Flagsprocess_TaskTCB;
//任务堆栈	
extern CPU_STK FLAGSPROCESS_TASK_STK[FLAGSPROCESS_STK_SIZE];
//任务函数
void flagsprocess_task(void *p_arg);

//任务优先级
#define MULTI_MAIN_TASK_PRIO		14
//任务堆栈大小	
#define MULTI_MAIN_STK_SIZE 		128
//任务控制块
extern OS_TCB MULTI_MAIN_TaskTCB;
//任务堆栈	
extern CPU_STK MULTI_MAIN_TASK_STK[MULTI_MAIN_STK_SIZE];
void multi_main_task(void *p_arg);

//任务优先级
#define MULTI_TASK_PRIO		15
//任务堆栈大小	
#define MULTI_STK_SIZE 		128
//任务控制块
extern OS_TCB Multi_TaskTCB;
//任务堆栈	
extern CPU_STK MULTI_TASK_STK[MULTI_STK_SIZE];
void multi_task(void *p_arg);

//任务优先级
#define MEMORY_MAIN_TASK_PRIO		16
//任务堆栈大小	
#define MEMORY_MAIN_STK_SIZE 		128
//任务控制块
extern OS_TCB MEMORY_MAIN_TaskTCB;
//任务堆栈	
extern CPU_STK MEMORY_MAIN_TASK_STK[MULTI_MAIN_STK_SIZE];
void memory_main_task(void *p_arg);

//任务优先级
#define MEMORY_TASK_PRIO		17
//任务堆栈大小	
#define MEMORY_STK_SIZE 		128
//任务控制块
extern OS_TCB MEMORY_TaskTCB;
//任务堆栈	
extern CPU_STK MEMORY_TASK_STK[MULTI_STK_SIZE];
void memory_task(void *p_arg);

//任务优先级
#define CAN1_TASK_PRIO		18
//任务堆栈大小	
#define CAN1_STK_SIZE 		128
//任务控制块
extern OS_TCB CAN1_TaskTCB;
//任务堆栈	
extern CPU_STK CAN1_TASK_STK[CAN1_STK_SIZE];
void can1_task(void *p_arg);
void can1_task_creat(void);

/* ==================== 任务开启 ======================== */
#define IS_OPEN_LED0_TASK   1
#define IS_OPEN_LED1_TASK   0
#define IS_OPEN_FLOAT_TASK  0
#define IS_OPEN_tmr1_TASK   0
#define IS_OPEN_tmr2_TASK   0
#define IS_OPEN_sem_TASK    0
#define IS_OPEN_mutex_test_TASK 0
#define IS_OPEN_message_queue_TASK 0
#define IS_OPEN_flag_TASK 0
#define IS_OPEN_pendmulti_TASK 0
#define IS_OPEN_memory_TASK 0
#define IS_OPEN_can1_TASK    1
/* ======================================================= */

/*           初始化函数                         */
void BSP_Init(void);
#endif



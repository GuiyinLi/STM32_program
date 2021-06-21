#include "task_start.h"
#include "includes.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "os_cfg.h"
#include <os_app_hooks.h>
#include "key.h"
#include "ucosiii_tmr.h"
#include "sem.h"
#include "message_queue.h"
#include "flag.h"
#include "pendmulti.h"
#include "memory.h"
#include "can.h"
OS_TCB StartTaskTCB;   //任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];   //任务堆栈
//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);  	//统计任务 ->os_cfg.h:包括一些基础任务的宏定义 空闲 时钟节拍 定时等5个任务               
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候 解决同优先级任务分配情况
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		

/*#if OS_CFG_APP_HOOKS_EN   //使能钩子函数->os_app_hooks.c
	App_OS_SetAllHooks();   //在空闲任务的钩子函数中不能出现让空闲任务进入等待态的代码 否则CPU会无所事事
#endif*/
  //定时器任务创建
	#if IS_OPEN_tmr1_TASK  //定时器
	tmr1_task_create();
	#endif
	
	#if IS_OPEN_tmr2_TASK
	tmr2_task_create();
	#endif

	OS_CRITICAL_ENTER();	//进入临界区
	
	#if IS_OPEN_sem_TASK   //互斥量
	sem_task_create();
	#endif
	
	#if IS_OPEN_LED0_TASK
	led0_task_create();
	#endif
	
	#if IS_OPEN_LED1_TASK
	led1_task_create();
	#endif
	
	#if IS_OPEN_FLOAT_TASK
	float_test_task_create();
	#endif
	
	#if IS_OPEN_mutex_test_TASK  //互斥信号量
	mutex_test_task_creat();
	#endif
	
	#if IS_OPEN_message_queue_TASK  //消息队列
	message_queue_task_creat();
	#endif
	
	#if IS_OPEN_flag_TASK   //事件标志位
	flag_task_create();
	#endif
	
	#if IS_OPEN_pendmulti_TASK  //等待多个内核对象
	pendmulti_task_creat();
	#endif
	
	#if IS_OPEN_memory_TASK   //内存块管理
	memory_task_creat();
	#endif
	
	#if IS_OPEN_can1_TASK  //开启can1任务
	can1_task_creat();
	#endif
	
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

//创建开始任务
//不能在中断服务程序中调用任务创建函数
void start_Task_create(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度(时钟节拍(OS_CFG_TICK_RATE_HZ=200)/10)*5ms  1:1个时间片->长度=n*5ms
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
}

void BSP_Init(void)
{
	delay_init();       //延时初始化
	LED_Init();         //LED初始化
	KEY_Init();
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	uart_init(115200);    //串口波特率设置
}



#include "led_task.h"
#include "led.h"
#include "usart.h"
OS_TCB LedTaskTCB;
CPU_STK LED_TASK_STK[LED_STK_SIZE];

void led_task(void *p_arg)
{
	OS_ERR err;          //用于返回错误信息
	CPU_SR_ALLOC();        //为了进入临界区而写的代码  
	p_arg = p_arg;
	while(1)
	{
		OS_CRITICAL_ENTER();//进入-临界区
		printf("led task running\r\n");
		OS_CRITICAL_EXIT();	//退出临界区	 
		LED=!LED;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms*/
	}

}

//创建LED0任务
void led_task_create(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"led task", 		    //任务名字
                 (OS_TASK_PTR )led_task,   //任务函数
                 (void		* )0,					    //传递给任务函数的参数
                 (OS_PRIO	  )LED_TASK_PRIO,   //任务优先级  
                 (CPU_STK   * )&LED_TASK_STK[0],		//任务堆栈基地址
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)LED_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					     //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
}



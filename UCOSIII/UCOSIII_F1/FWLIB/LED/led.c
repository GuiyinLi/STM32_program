#include "led.h"
#include "stm32f10x.h"
#include "task_start.h"
#include "delay.h"
#include "key.h"
#include "ucosiii_tmr.h"
#include "usart.h"
#include "sem.h"
#include "message_queue.h"
#include "can.h"
u8 led0_task_str[]="led0 running";
u8 led1_task_str[]="led1 running";
float float_num=0.01;
OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

OS_TCB Led1TaskTCB;
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];

 OS_TCB	FloatTaskTCB;
__align(8) CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOB, &GPIO_InitStructure);					
  GPIO_SetBits(GPIOB,GPIO_Pin_0);						

	
}

//led0任务函数
void led0_task(void *p_arg)
{
	static u8 flag=0;
	static u16 led0_task_num=0;
	u8 canbuf[8]={0,1,2,3,4,5,6,7};
	u8 key,*pbuf,size;
	OS_ERR err;          //用于返回错误信息
	
	CPU_SR_ALLOC();        //为了进入临界区而写的代码  
	p_arg = p_arg;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)
		{
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,1);
			flag=1;
			
		}
		else if(key==KEY0_PRES)
		{
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,0);
			flag=0;
		}
		if(flag)
		{
			Can_Send_Msg(canbuf,8);         //发送八个字节
			flag=0;
		}
//		if(key==KEY1_PRES)
//		{
//			OSTaskSemPost(&Led1TaskTCB,OS_OPT_POST_1,&err);
//			printf("led0_task:%d\r\n",Led1TaskTCB.SemCtr);
//			//OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//请求信号量 当Ctr>1时 才能请求成功 每成功一次 Ctr-1 请求失败 则不再执行后面的语句 当为0时 无论请求多少次 最小为-1 一旦检测到大于零 则立马执行任务
//		  //printf("led0_task:%d\r\n",SYNC_SEM.Ctr);      //显示信号量值
//			
//		}

		//sem_share(led0_task_str);
		/*led0_task_num++;
		printf("led0_task已执行%d次\r\n",led0_task_num);
		if(led0_task_num==5)
		{
			//OSTaskDel((OS_TCB*)&Led1TaskTCB,&err);	//任务1执行5此后删除掉任务2  第一个参数为0则删除自身(不加&)
			//printf("led0_task Del led1_task\r\n");
			OS_TaskSuspend((OS_TCB*)&Led1TaskTCB,&err);		//挂起led1_task
			printf("led0_task Suspend led1_task\r\n");
			
		}
		else if(led0_task_num==10)
		{
			OS_TaskResume((OS_TCB*)&Led1TaskTCB,&err);		//挂起led1_task  为0则挂起自身
			printf("led0_task Resume led1_task\r\n");
		}*/
		
		/*LED0=0;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED0=1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms*/
	}
}

//led1任务函数
void led1_task(void *p_arg)
{
	static u16 led1_task_num=0;
	OS_ERR err;          //用于返回错误信息
	u8 key;
	CPU_SR_ALLOC();        //为了进入临界区而写的代码  
	p_arg = p_arg;
	while(1)
	{
		//sem_share(led1_task_str);
		key = KEY_Scan(0);
		if(key==KEY0_PRES)   //按键一 开启定时器1
		{
			OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
			printf("led0_task:%d\r\n",Led1TaskTCB.SemCtr);
			
			//OSSemPost (&SYNC_SEM,OS_OPT_POST_1,&err);				//发送信号量 每发送一次 信号量+1
			//printf("led1_task:%d\r\n",SYNC_SEM.Ctr);      //显示信号量值
			/*OSTmrStart(&tmr1,&err);
			OS_CRITICAL_ENTER();	//进入临界区
			printf("开启定时器1\r\n");
			OS_CRITICAL_EXIT();		//退出临界区*/
		}
		else if(key==KEY1_PRES)  //2开启定时器2  单次定时器 运行完自动关闭
		{
			/*OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err);	//关闭定时器1
      OSTmrStart(&tmr2,&err);
			OS_CRITICAL_ENTER();	//进入临界区
			printf("关闭定时器1\r\n开启定时器2\r\n");
			OS_CRITICAL_EXIT();		//退出临界区*/
		}
		LED1=!LED1;
	}
}

//浮点测试任务
void float_task(void *p_arg)
{
	CPU_SR_ALLOC();        //为了进入临界区而写的代码  
	 p_arg = p_arg;             //防止编译器警告	
	while(1)
	{
		float_num+=0.01f;
		OS_CRITICAL_ENTER();	//进入临界区
		printf("Float:float_num的值为: %.4f\r\n",float_num);
		OS_CRITICAL_EXIT();		//退出临界区
		delay_ms(1000);			//延时500ms   //如果把延时函数放在临界区内 会一直打印数据 间隔无效果
	}
}

//创建LED0任务
void led0_task_create(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		//任务控制块
				 (CPU_CHAR	* )"led0 task", 		    //任务名字
                 (OS_TASK_PTR )led0_task,   //任务函数
                 (void		* )0,					    //传递给任务函数的参数
                 (OS_PRIO	  )LED0_TASK_PRIO,   //任务优先级  
                 (CPU_STK   * )&LED0_TASK_STK[0],		//任务堆栈基地址
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)LED0_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					     //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
}

//创建LED1任务
void led1_task_create(void)
{
  OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     	
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
}

//创建浮点测试任务
void float_test_task_create(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&FloatTaskTCB,		
				 (CPU_CHAR	* )"float test task", 		
                 (OS_TASK_PTR )float_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )FLOAT_TASK_PRIO,     	
                 (CPU_STK   * )&FLOAT_TASK_STK[0],	
                 (CPU_STK_SIZE)FLOAT_STK_SIZE/10,	
                 (CPU_STK_SIZE)FLOAT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
}



#include "sem.h"
#include "delay.h"
#include "task_start.h"
#include "led.h"
OS_TCB High_TaskTCB;
CPU_STK HIGH_TASK_STK[HIGH_STK_SIZE];

OS_TCB Middle_TaskTCB;
CPU_STK MIDDLE_TASK_STK[MIDDLE_STK_SIZE];

OS_TCB Low_TaskTCB;
CPU_STK LOW_TASK_STK[LOW_STK_SIZE];
/* =====================相关函数======================= */
/*                                                       |
    OSSemCreate()	           //创建一个信号量            |
		OSSemDel()	             //删除信号量                |
		OSSemPend()	             //等待信号量                |
		OSSemPendAbort()	       //取消等待                  |
		OSSemPost()	             //释放信号量                |
		OSSemSet()	             //强制设置一个信号量        |
		//互斥信号量                                         |
		OSMutexCreate()	           //创建一个信号量          |
		OSMutexDel()	             //删除信号量              |
		OSMutexPend()	             //等待信号量              |
		OSMutexPendAbort()	       //取消等待                |
		OSMutexPost()	             //释放信号量              |
		//任务内嵌信号量                                     |
		OSTaskSemPend()	           //创建一个信号量          |
		OSTaskSemPendAbort()	     //取消等待                |
		OSTaskSemPost()	           //释放信号量              |
		OSTaskSemSet()             //强行设置任务信号量计数  |
*/
/* ==================================================== */

/*                      信号量类型                         */
/* OS_SEM_CTR=1->二进制信号量 只允许一个任务访问共享资源 
   OS_SEM_CTR=n->计数型信号量 允许n个任务同时访问共享资源
	 互斥型信号量 防止优先级反转 无0 1 n之分
	 任务内嵌信号量 此信号量不用创建 可实现任务同步
*/

/*   信号量请求类型 OSSemPend->决定任务是否执行   */
/*
	OS_OPT_PEND_BLOCKING     指定信号量无效时 任务挂起以等待信号量	 挂起时 该任务不再执行
	OS_OPT_PEND_NON_BLOCKING 指定信号量无效时 任务直接返回  任务仍可执行
*/
/* ================= 信号量用途 =========================== */
/*
   1.实现任务共享资源: 每个资源都有对应的信号量 只有当一个任
     务获得该资源的信号量 才可对资源进行访问
	 2.实现任务同步: 请求不到信号量 则不执行任务 当另一个任务
	   发送信号量该任务才会请求到信号量 即该任务由另一个任务触发
	 3.优先级反转:当有三个任务时 低级任务有信号量 中级无 高级有
	   信号量 高级等待低级释放信号量才可执行 在此期间 中级任务可
		 打断低级任务执行 优先于高级任务执行 而互斥信号量可解决优
		 先级反转问题
*/
/* ======================================================== */
u8 share_resource[30];   //共享资源区
OS_SEM	MY_SEM;	
OS_SEM	SYNC_SEM;	
OS_MUTEX	TEST_MUTEX;
void sem_task_create(void)
{
	OS_ERR err;
	OSSemCreate ((OS_SEM*	)&MY_SEM,
                 (CPU_CHAR*	)"MY_SEM",
                 (OS_SEM_CTR)1,		
                 (OS_ERR*	)&err);
	
	OSSemCreate ((OS_SEM*	)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",
                 (OS_SEM_CTR)0,		             //初始信号量为0
                 (OS_ERR*	)&err);
	OSMutexCreate((OS_MUTEX*	)&TEST_MUTEX,   //互斥信号量
				  (CPU_CHAR*	)"TEST_MUTEX",
                  (OS_ERR*		)&err);
}

void sem_share(char str[])
{
	OS_ERR err;
	OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//请求信号量
	memcpy(share_resource,str,strlen(str)); //向共享资源区拷贝数据
	delay_ms(300);
	printf("%s\r\n",share_resource);	//串口输出共享资源区数据	
	OSSemPost (&MY_SEM,OS_OPT_POST_1,&err);				//发送信号量
}

void mutex_test_task_creat()
{
	OS_ERR err;
	//创建HIGH任务
	OSTaskCreate((OS_TCB 	* )&High_TaskTCB,		
				 (CPU_CHAR	* )"High task", 		
                 (OS_TASK_PTR )high_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )HIGH_TASK_PRIO,     
                 (CPU_STK   * )&HIGH_TASK_STK[0],	
                 (CPU_STK_SIZE)HIGH_STK_SIZE/10,	
                 (CPU_STK_SIZE)HIGH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//创建MIDDLE任务
	OSTaskCreate((OS_TCB 	* )&Middle_TaskTCB,		
				 (CPU_CHAR	* )"Middle task", 		
                 (OS_TASK_PTR )middle_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MIDDLE_TASK_PRIO,     
                 (CPU_STK   * )&MIDDLE_TASK_STK[0],	
                 (CPU_STK_SIZE)MIDDLE_STK_SIZE/10,	
                 (CPU_STK_SIZE)MIDDLE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);		
	//创建LOW任务
	OSTaskCreate((OS_TCB 	* )&Low_TaskTCB,		
				 (CPU_CHAR	* )"Low task", 		
                 (OS_TASK_PTR )low_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LOW_TASK_PRIO,     
                 (CPU_STK   * )&LOW_TASK_STK[0],	
                 (CPU_STK_SIZE)LOW_STK_SIZE/10,	
                 (CPU_STK_SIZE)LOW_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);					 
}

//高优先级任务的任务函数
void high_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);   		//延时500ms
		printf("high task Pend Mutex\r\n");
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//请求互斥信号量
		printf("high task Running!\r\n");
		LED1 = ~LED1;
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);   		//延时500ms
	}
}

//中等优先级任务的任务函数
void middle_task(void *p_arg)
{	
	OS_ERR err;
	
	while(1)
	{
		printf("middle task Running!\r\n");
		LED0 = ~LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   //延时1s
	}
}

//低优先级任务的任务函数
void low_task(void *p_arg)
{	
	static u32 times;
	OS_ERR err;
	while(1)
	{
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);//请求互斥信号量
		printf("low task Running!\r\n");
		for(times=0;times<10000000;times++)
		{
			OSSched();											//发起任务调度
		}
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);			//释放互斥信号量
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);   	//延时1s
	}
}

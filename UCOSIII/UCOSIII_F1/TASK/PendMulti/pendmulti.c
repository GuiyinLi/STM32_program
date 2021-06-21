#include "pendmulti.h"
#include "led.h"
#include "key.h"
#include "task_start.h"
//同时等待多个内核对象->多个消息队列和信息量 不能等待多个互斥信号量个事件标志组 OS_CFG_PEND_MULTI_EN=1u
OS_TCB MULTI_MAIN_TaskTCB;
CPU_STK MULTI_MAIN_TASK_STK[MULTI_MAIN_STK_SIZE];

OS_TCB Multi_TaskTCB;
CPU_STK MULTI_TASK_STK[MULTI_STK_SIZE];

OS_SEM	Test_Sem1;			//信号量1		
OS_Q	Test_Q;				//消息队列	
void pendmulti_task_creat(void)
{
	OS_ERR err;
	//创建信号量Test_Sem1
	OSSemCreate ((OS_SEM*	)&Test_Sem1,
                 (CPU_CHAR*	)"Test_Sem1",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建消息队列
	OSQCreate ((OS_Q*		)&Test_Q,	//消息队列
                (CPU_CHAR*	)"KEY Msg",	//消息队列名称
                (OS_MSG_QTY	)QUEUE_NUM,	//消息队列长度
                (OS_ERR*	)&err);		//错误码
	//创建TASK1任务
	OSTaskCreate((OS_TCB 	* )&MULTI_MAIN_TaskTCB,		
				 (CPU_CHAR	* )"Multi main task", 		
                 (OS_TASK_PTR )multi_main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MULTI_MAIN_TASK_PRIO,     
                 (CPU_STK   * )&MULTI_MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MULTI_MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MULTI_MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
	//创建MULTI测试任务
	OSTaskCreate((OS_TCB 	* )&Multi_TaskTCB,		
				 (CPU_CHAR	* )"Multi task", 		
                 (OS_TASK_PTR )multi_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MULTI_TASK_PRIO,     
                 (CPU_STK   * )&MULTI_TASK_STK[0],	
                 (CPU_STK_SIZE)MULTI_STK_SIZE/10,	
                 (CPU_STK_SIZE)MULTI_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
}
//任务1的任务函数
void multi_main_task(void *p_arg)
{
	u8 key;
	OS_ERR err;
	char *pbuf;
	static u8 msg_num;
	
	while(1)
	{
		key = KEY_Scan(0);  //扫描按键
		switch(key)
		{
			case KEY1_PRES:
				OSSemPost(&Test_Sem1,OS_OPT_POST_1,&err);//发送信号量1
				break;
			case KEY0_PRES:
				pbuf=(char *)malloc(8);	//申请内存
				msg_num++;
				sprintf((char*)pbuf,"multi %d",msg_num);
				//发送消息
				OSQPost((OS_Q*		)&Test_Q,		
						(void*		)pbuf,
						(OS_MSG_SIZE)10,
						(OS_OPT		)OS_OPT_POST_FIFO,
						(OS_ERR*	)&err);
						if(err != OS_ERR_NONE) //要采用malloc 和free来完后消息的发送和接收 不然之前的消息会被最近的消息覆盖 效果体现在LIFO 和 FIFO的选择 若不设置为ALL 则将消息发送给等待任务中优先级最高的任务
		        {
							free(pbuf);	//释放内存
						  printf("messag queue have fulled\r\n");  //提示消息队列已满
		        }
				break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}

//等待多个内核对象的任务函数
void multi_task(void *p_arg)
{	
	OS_ERR err;
	OS_OBJ_QTY index=0;   ///记录请求对象数目
	OS_PEND_DATA pend_multi_tbl[CORE_OBJ_NUM];	   //定义内核对象数组
	
	pend_multi_tbl[0].PendObjPtr=(OS_PEND_OBJ*)&Test_Sem1;  //初始化内核对象
	pend_multi_tbl[1].PendObjPtr=(OS_PEND_OBJ*)&Test_Q;
	
	while(1)
	{
		index=OSPendMulti((OS_PEND_DATA*	)pend_multi_tbl,	
						  (OS_OBJ_QTY		)CORE_OBJ_NUM,	//等待内核数量
						  (OS_TICK		   	)0,		   //超时值
						  (OS_OPT         	)OS_OPT_PEND_BLOCKING,
						  (OS_ERR*			)&err);
		printf("REQUST OBJ NUM:%d",index);
		if(pend_multi_tbl[1].RdyObjPtr==(OS_PEND_OBJ*)&Test_Q)  //pend_multi_tbl[n]. 后包含各个内核对象的请求信息
		{
			printf("%s\r\n",pend_multi_tbl[1].RdyMsgPtr);
		}
		else if(pend_multi_tbl[0].PendObjPtr==(OS_PEND_OBJ*)&Test_Sem1)
		{
			printf("Test Sem1\r\n");
		}
		LED0=!LED0;
		
	}
}



	



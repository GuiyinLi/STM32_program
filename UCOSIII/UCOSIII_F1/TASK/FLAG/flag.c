#include "flag.h"
#include "led.h"
#include "key.h"
#include "task_start.h"

//事件标志组：用于进行多个任务同步实验
/* =====================相关函数======================= */
/*                                                       |
    OSFlagCreate()	         //创建一个事件标志组        |
		OSFlagDel()	             //删除                      |
		OSFlagPend()	             //等待                    |
		OSFlagPendAbort()	       //取消等待                  |
		OSFlagPost()	             //发布标志                |
		OSFlagPendGetFlagsRdy()//获取使任务就绪的事件标志    |
*/
/* ==================================================== */

/* =================== OSFlagPend =================== */
/*
	OS_OPT_PEND_FLAG_SET_ALL  //等待所有标志位置位
	OS_OPT_PEND_FLAG_CLR_ALL  //等待所有标志位清零
	OS_OPT_PEND_FLAG_SET_ANY  //等待任何标志位置位
	OS_OPT_PEND_FLAG_CLR_ANY  //等待任何标志位清零
	
	OS_OPT_PEND_FLAG_CONSUME  //设置是否保留该事件标志的状态
	OS_OPT_PEND_NON_BLOCKING  //标志位不满足任务时不挂起任务
	OS_OPT_PEND_BLOCKING      //标志位不满足任务时挂起任务
*/
/* ================================================== */
OS_FLAG_GRP	EventFlags;		//定义一个事件标志组

OS_TCB FLAG_Main_TaskTCB;
CPU_STK FLAG_MAIN_TASK_STK[MAIN_STK_SIZE];

OS_TCB Flagsprocess_TaskTCB;
CPU_STK FLAGSPROCESS_TASK_STK[FLAGSPROCESS_STK_SIZE];
void flag_task_create()
{
	OS_ERR err;
	//创建一个事件标志组
	OSFlagCreate((OS_FLAG_GRP*)&EventFlags,		//指向事件标志组
                 (CPU_CHAR*	  )"Event Flags",	//名字
                 (OS_FLAGS	  )KEYFLAGS_VALUE,	//事件标志组初始值
                 (OS_ERR*  	  )&err);			//错误码
	//创建主任务
	OSTaskCreate((OS_TCB*     )&FLAG_Main_TaskTCB,		
				 (CPU_CHAR*   )"Flag Main task", 		
                 (OS_TASK_PTR )flag_main_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )FLAG_MAIN_TASK_PRIO,     
                 (CPU_STK*    )&FLAG_MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)FLAG_MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)FLAG_MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);						
	//创建MSGDIS任务
	OSTaskCreate((OS_TCB*     )&Flagsprocess_TaskTCB,		
				 (CPU_CHAR*   )"Flagsprocess task", 		
                 (OS_TASK_PTR )flagsprocess_task, 			
                 (void* 	  )0,					
                 (OS_PRIO	  )FLAGSPROCESS_TASK_PRIO,     
                 (CPU_STK* 	  )&FLAGSPROCESS_TASK_STK[0],	
                 (CPU_STK_SIZE)FLAGSPROCESS_STK_SIZE/10,	
                 (CPU_STK_SIZE)FLAGSPROCESS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void* 	  )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR* 	  )&err);
}

//主任务的任务函数
void flag_main_task(void *p_arg)
{
	u8 key;
	OS_FLAGS flags_num;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0);  //扫描按键
		if(key == KEY0_PRES)
		{
			//向事件标志组EventFlags发送标志
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
								 (OS_FLAGS	  )KEY0_FLAG,
								 (OS_OPT	  )OS_OPT_POST_FLAG_SET,    //对应标志位置位 OS_OPT_POST_FLAG_CLR 对标志位清零
					       (OS_ERR*	  )&err);
			printf("事件标志组EventFlags的值:%d\r\n",flags_num);
		}
		else if(key == KEY1_PRES)
		{
			//向事件标志组EventFlags发送标志
			flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
								 (OS_FLAGS	  )KEY1_FLAG,                 
								 (OS_OPT	  )OS_OPT_POST_FLAG_SET,    //对应标志位置位
								 (OS_ERR*     )&err);
			printf("事件标志组EventFlags的值:%d\r\n",flags_num);
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}

//事件标志组处理任务
void flagsprocess_task(void *p_arg)
{
	OS_ERR err; 
	while(1)
	{
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
				   (OS_FLAGS	)KEY0_FLAG+KEY1_FLAG,
		     	   (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME+OS_OPT_PEND_BLOCKING,  //等待所有标志位置位 清空上次记录(不清空会一直触发)
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		LED1 = ~LED1;    //两个按键都按下 才可触发该事件
		printf("事件标志组EventFlags的值:%d\r\n",EventFlags.Flags);
	}
}



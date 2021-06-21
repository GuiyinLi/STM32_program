#include "can_task.h"
#include "task_start.h"
#include "can.h"
#include "led.h"
u8 can1_send_buf[8];
OS_TCB CAN1_TaskTCB;
CPU_STK CAN1_TASK_STK[CAN1_STK_SIZE];

OS_SEM	CAN1_SEM;

void can1_task_creat(void)
{
	OS_ERR err;
	OSSemCreate ((OS_SEM*	)&CAN1_SEM,
                 (CPU_CHAR*	)"CAN1_SEM",
                 (OS_SEM_CTR)0,
                 (OS_ERR*	)&err);
	OSTaskCreate((OS_TCB 	* )&CAN1_TaskTCB,		
				 (CPU_CHAR	* )"can1 task", 		
                 (OS_TASK_PTR )can1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CAN1_TASK_PRIO,     	
                 (CPU_STK   * )&CAN1_TASK_STK[0],	
                 (CPU_STK_SIZE)CAN1_STK_SIZE/10,	
                 (CPU_STK_SIZE)CAN1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
}

void can1_task(void *p_arg)
{
	OS_ERR err;
	u8 i=0;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&CAN1_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量  等待中断发送信号量
		OS_CRITICAL_ENTER();	//进入临界区
		switch(RxMessage.StdId)
		{
			case 0x201:
			{
				LED0=!LED0;
				printf("Receive Message:");
				for(i=0;i<8;i++)
			  {
				  printf("%d ",RxMessage.Data[i]);
					can1_send_buf[i]=RxMessage.Data[i];
			  }
				printf("\r\n");
				//Can_Send_Msg(can1_send_buf,8);
				break;
			}
			case 0x202:
			{
				break;
			}
			default:
				break;
		}
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms*/
		OS_CRITICAL_EXIT();	//进入临界区
	}
}

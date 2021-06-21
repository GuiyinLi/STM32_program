#include "message_queue.h"
#include "task_start.h"
#include "key.h"
#include "led.h"
#include "usart.h"
OS_Q KEY_Msg;
OS_TCB Keyprocess_TaskTCB;
CPU_STK KEYPROCESS_TASK_STK[KEYPROCESS_STK_SIZE];

OS_Q DATA_Msg;				//定义一个消息队列，用于发送数据

OS_TCB Main_TaskTCB;
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];

/* =====================相关函数======================= */
/*                                                       |
    OSQCreate()	           //创建一个消息队列            |
		OSQDel()	             //删除消息队列                |
		OSQFlush()	             //清空消息队列              |
		OSQPend()	            //等待消息队列                 |
		OSQPendPost()	        //取消等待消息队列             |
		OSQPost()	            //向消息队列发送一条消息       |
		//任务内建消息队列                                   |->OS_CFG_TASK_Q_EN=1
		OSTaskQPend()	           //创建一个消息队列          |
		OSTaskQPendAbort()	     //取消等待                  |
		OSTaskQPost()	           //发送消息                  |
		OSTaskQFlush()            //清空任务的消息队列       |
*/
/* ==================================================== */
void message_queue_task_creat()
{
	OS_ERR err;
	//创建消息队列KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//消息队列
                (CPU_CHAR*	)"KEY Msg",	//消息队列名称
                (OS_MSG_QTY	)KEYMSG_Q_NUM,	//消息队列长度，这里设置为1 必须大于零
                (OS_ERR*	)&err);		//错误码
		//创建消息队列DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)DATAMSG_Q_NUM,	
                (OS_ERR*	)&err);	
	//创建按键任务
	OSTaskCreate((OS_TCB 	* )&Keyprocess_TaskTCB,		
				 (CPU_CHAR	* )"Keyprocess task", 		
                 (OS_TASK_PTR )Keyprocess_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEYPROCESS_TASK_PRIO,     
                 (CPU_STK   * )&KEYPROCESS_TASK_STK[0],	
                 (CPU_STK_SIZE)KEYPROCESS_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEYPROCESS_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM,		//任务MsgKeyprocess_Task需要使用内建消息队列，消息队列长度为4						
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
  OSTaskCreate((OS_TCB 	* )&Main_TaskTCB,		
				 (CPU_CHAR	* )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK   * )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);									 
}
//按键处理任务的任务函数
void Keyprocess_task(void *p_arg)
{	
	u8 *key,*pbuf;
	OS_MSG_SIZE size;
	OS_ERR err;
	while(1)
	{
		//请求消息KEY_Msg
		key=OSQPend((OS_Q*			)&KEY_Msg,   
					(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING, //阻塞模式
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,                 //时间戳
                    (OS_ERR*		)&err);
		switch(*key)
		{
			case KEY1_PRES:		
				pbuf=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );
			printf("Keyprocess_TaskTCB::%s\r\n",pbuf);  //打印接收到的信息
		  free(pbuf);  //释放内存
				break;
			case KEY0_PRES:		
				/*pbuf=OSQPend((OS_Q*			)&DATA_Msg,   
					(OS_TICK		)0,
										 (OS_OPT			)OS_OPT_PEND_BLOCKING, //阻塞模式
										 (OS_MSG_SIZE*	)&size,		
										 (CPU_TS*		)0,                 //时间戳
										 (OS_ERR*		)&err);
		printf("DATA_Msg:%s\r\n",pbuf);  //打印接收到的信息
		free(pbuf);  //释放内存*/
				break;
		}
	}
}

//主任务的任务函数
void main_task(void *p_arg)
{
	static u8 num=0;
	char *message_test;
	char *q;
	u8 key;
	OS_ERR err;
	
	while(1)
	{
		key = KEY_Scan(0);  //扫描按键
		if(key)
		{
			
			//发送消息
			OSQPost((OS_Q*		)&KEY_Msg,		
					(void*		)&key,               //发送内容
					(OS_MSG_SIZE)1,                //发送字节数
					(OS_OPT		)OS_OPT_POST_FIFO,   //消息发送操作类型
					(OS_ERR*	)&err);
		//发送消息->任一按键按下 发送队列信息
					if(key==KEY1_PRES)
					{
						/*message_test=(char *)malloc(10);  //调用malloc时 要把魔术棒里的Use MicroLIB 勾选
						num++;
						sprintf((char*)message_test,"message:%d",num);	//显示DATA_Msg消息队列总的大小 
						OSQPost((OS_Q*		)&DATA_Msg,		
								(void*		)message_test,
								(OS_MSG_SIZE)10,
								(OS_OPT		)OS_OPT_POST_FIFO+OS_OPT_POST_ALL,  //LIFO 后进先出  FIFO 先进先出  ALL：将消息发送给所有等待的任务 配合FIFO和LIFO使用 默认是FIFO
								(OS_ERR*	)&err);
						if(err != OS_ERR_NONE) //要采用malloc 和free来完后消息的发送和接收 不然之前的消息会被最近的消息覆盖 效果体现在LIFO 和 FIFO的选择 若不设置为ALL 则将消息发送给等待任务中优先级最高的任务
		        {
							free(message_test);	//释放内存
						  printf("DATA_Msg:messag queue have fulled\r\n");  //提示消息队列已满
		        }
						check_msg_queue(DATA_Msg);  //显示消息队列总数量和剩余数量*/

					}
					else if(key==KEY0_PRES)
					{
						q=(char *)malloc(10);  //调用malloc时 要把魔术棒里的Use MicroLIB 勾选
						num++;
						sprintf((char*)q,"message:%d",num);	//显示DATA_Msg消息队列总的大小 
						OSTaskQPost((OS_TCB*	)&Keyprocess_TaskTCB,	//Keyprocess_TaskTCB发送消息
                    (void*		)q,
                    (OS_MSG_SIZE)10,
                    (OS_OPT		)OS_OPT_POST_LIFO,
					  (OS_ERR*	)&err);
						if(err != OS_ERR_NONE) //要采用malloc 和free来完后消息的发送和接收 不然之前的消息会被最近的消息覆盖 效果体现在LIFO 和 FIFO的选择 若不设置为ALL 则将消息发送给等待任务中优先级最高的任务
		        {
							free(q);	//释放内存
							printf("messag queue have fulled\r\n");  //提示消息队列已满
		        }
						printf("Keyprocess_TaskTCB:Total Size:%d  remain Size:%d\r\n",Keyprocess_TaskTCB.MsgQ.NbrEntriesSize,Keyprocess_TaskTCB.MsgQ.NbrEntriesSize-Keyprocess_TaskTCB.MsgQ.NbrEntries);
					}
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms  延时必须加 不然接收不到消息
		
	}
}

//查询DATA_Msg消息队列中的总队列数量和剩余队列数量
void check_msg_queue(OS_Q Msg)
{
	CPU_SR_ALLOC();
	u8 msgq_remain_size;	//消息队列剩余大小
	OS_CRITICAL_ENTER();	//进入临界段
	msgq_remain_size = Msg.MsgQ.NbrEntriesSize-Msg.MsgQ.NbrEntries; //剩余量=总量-使用量
	printf("Total Size:%d  remain Size:%d\r\n",DATA_Msg.MsgQ.NbrEntriesSize,msgq_remain_size);
	OS_CRITICAL_EXIT();		//退出临界段
}

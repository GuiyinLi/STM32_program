#include "memory.h"
#include "task_start.h"
#include "key.h"

/* =====================相关函数======================= */
/*                                                       |
    OSMemCreate()           //创建一个存储分区           |
		OSMemGet()	             //从存储区中获得一个存储块  |
		OSMemPut()	             //归还存储块                |
*/
/* ==================================================== */

// 优点：代替malloc()和free(),方式存储区碎片化 导致不能分配到合适大小的连续存储区
// 缺点：灵活性不够好 只能以块为单位进行分配和释放 每块的大小是固定的

//注意：不要在内存区满之后再释放内存 此时会造成内存泄露
OS_MEM INTERNAL_MEM;
__align(4) CPU_INT08U Internal_RamMemp[INTERNAL_MEM_NUM][INTERNAL_MEMBLOCK_SIZE];

OS_TCB MEMORY_MAIN_TaskTCB;
CPU_STK MEMORY_MAIN_TASK_STK[MULTI_MAIN_STK_SIZE];

OS_TCB MEMORY_TaskTCB;
CPU_STK MEMORY_TASK_STK[MULTI_STK_SIZE];
 
void memory_task_creat()
{
	OS_ERR err;
	//创建一个存储分区
	OSMemCreate((OS_MEM*	)&INTERNAL_MEM,
				(CPU_CHAR*	)"Internal Mem",
				(void*		)&Internal_RamMemp[0][0],  //存储区基地址
				(OS_MEM_QTY	)INTERNAL_MEM_NUM,       //存储块数量
				(OS_MEM_SIZE)INTERNAL_MEMBLOCK_SIZE, //块大小
				(OS_ERR*	)&err);
		//创建主任务
	OSTaskCreate((OS_TCB*     )&MEMORY_MAIN_TaskTCB,		
				 (CPU_CHAR*   )"Main task", 		
                 (OS_TASK_PTR )memory_main_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MEMORY_MAIN_TASK_PRIO,     
                 (CPU_STK*    )&MEMORY_MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MEMORY_MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MEMORY_MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//创建一个内存块检查任务
	OSTaskCreate((OS_TCB*     )&MEMORY_TaskTCB,		
				 (CPU_CHAR*   )"MemManage task", 		
                 (OS_TASK_PTR )memory_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MEMORY_TASK_PRIO,     
                 (CPU_STK*    )&MEMORY_TASK_STK[0],	
                 (CPU_STK_SIZE)MEMORY_STK_SIZE/10,	
                 (CPU_STK_SIZE)MEMORY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
}

//主任务的任务函数
void memory_main_task(void *p_arg)
{
	u8 key;
	static u8 internal_memget_num=0;
	CPU_INT08U *internal_buf;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0);  //扫描按键
		switch(key)
		{
			case KEY0_PRES:
				internal_buf=OSMemGet((OS_MEM*)&INTERNAL_MEM,
								      (OS_ERR*)&err);
				if(err == OS_ERR_NONE) //内存申请成功
				{
					printf("internal_buf内存申请之后的地址为:%#x\r\n",(u32)(internal_buf));
					internal_memget_num++;
					printf("INTERNAL_MEM Use %d times\r\n",internal_memget_num);
				}
				if(err == OS_ERR_MEM_NO_FREE_BLKS) //内存块不足
				{
					printf("INTERNAL_MEM Empty!\r\n");
				}
				break;
			case KEY1_PRES:
				if(internal_buf != NULL) //internal_buf不为空就释放内存
				{
					OSMemPut((OS_MEM*	)&INTERNAL_MEM,		//释放内存
							 (void*		)internal_buf,
							 (OS_ERR* 	)&err);
					printf("internal_buf内存释放之后的地址为:%#x\r\n",(u32)(internal_buf));
							 printf("Memory Put success!\r\n");
				}
				break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}

//内存管理任务
void memory_task(void *p_arg)
{
	OS_ERR err;
	static u16 last_total=0;
	static u16 last_remain=0;
	while(1)
	{
		if(INTERNAL_MEM.NbrMax!=last_total)
		{
			printf("Total: %d\r\n",INTERNAL_MEM.NbrMax);
			last_total=INTERNAL_MEM.NbrMax;
		}
		if(INTERNAL_MEM.NbrFree!=last_remain)
		{
			printf("Remain: %d\r\n",INTERNAL_MEM.NbrFree);
			last_remain=INTERNAL_MEM.NbrFree;
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//延时100ms
	}
}

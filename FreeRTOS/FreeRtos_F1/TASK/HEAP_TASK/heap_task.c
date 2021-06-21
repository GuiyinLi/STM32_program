#include "heap_task.h"
#include "key.h"

/* ====================== 内存管理相关的API函数 =================== */
/*
	pvPortMalloc()         申请内存                                    |
	vPortFree()            释放内存                                    |
	heap_1.c:          只能申请 不能释放                               |
	heap_2.c:          能申请 能释放 容易产生碎片                      |
	heap_3.c:          对malloc()和free()进行了封装 提供了线程保护     |
	heap_4.c:          提供内存合并算法 减小了内存碎片化的情况         |
	heap_5.c:          基本和4相同 但可允许内存堆跨越多个不连续的内存区|
	                   例如既可以使用内部RAM也可使用外部RAM,           |
										 其他只能使用其中之一                            |
	以上函数的效率均高于malloc()和free()                               |
*/
/* ================================================================ */

TaskHandle_t HeapTask_Handler;


void heap_task(void *p_arg)
{
	u8 *buffer;		
	u8 times,key=0;
	printf("remain heap size : %d\r\n",xPortGetFreeHeapSize());  //获取剩余内存大小
  while(1)
  {
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES:				
				buffer=pvPortMalloc(30);			//申请内存，30个字节 会另外加上一个结构体大小和8字节对齐大小 一共是10字节大小
				printf("申请到的内存地址为:%#x\r\n",(int)buffer);
			  if(buffer!=NULL)					//buffer可用,使用buffer
				{
					times++;
					sprintf((char*)buffer,"User %d Times",times);//向buffer中填写一些数据
					printf("buffer:%s\r\n",buffer);
				}
				printf("remain heap size : %d\r\n",xPortGetFreeHeapSize());  //获取剩余内存大小
				break;
			case KEY1_PRES:				
				if(buffer!=NULL)vPortFree(buffer);	//释放内存
				buffer=NULL;
			  printf("free heap\r\n");
			  printf("remain heap size : %d\r\n",xPortGetFreeHeapSize());  //获取剩余内存大小
				break;
		}
  }
}

//开始任务任务函数
void heap_task_creat(void)
{
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )heap_task,             
                (const char*    )"heap_task",           
                (uint16_t       )HEAP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )HEAP_TASK_PRIO,        
                (TaskHandle_t*  )&HeapTask_Handler);   
}

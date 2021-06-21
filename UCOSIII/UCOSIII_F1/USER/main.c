#include "stm32f10x.h" 
#include "sys.h" 
#include "key.h"
#include "delay.h"
#include "led.h"
#include "task_start.h"
#include "usart.h"
#include "can.h"
int main(void)
{
	
	OS_ERR err;
	CPU_SR_ALLOC();
	
	BSP_Init();
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入-临界区
	
	start_Task_create();	//创建开始任务
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}




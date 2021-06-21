#include "led_task.h"
#include "led.h"
#include "delay.h"
TaskHandle_t LED0Task_Handler;
//LED0任务函数 
void led0_task(void *pvParameters)
{
	//u16 led0_task_num=0;
    while(1)
    {
//			led0_task_num++;
//			taskENTER_CRITICAL();           //进入临界区->任务级   taskENTER_CRITICAL_From_ISR->中断级
//			printf("led0_task run %d times\r\n",led0_task_num);
//			taskEXIT_CRITICAL();            //退出临界区      taskEXIT_CRITICAL_From_ISR->中断级
//			delay_ms(10);
			// portDISABLE_INTERRUPTS();   //关闭5-15之间的中断
			 // printf("led0 task running\r\n");
        LED0=~LED0;
			  delay_ms(500);               //delay_xms()不会引起任务调度 5-15之间的中断不会运行 等到再次开启中断后才会运行
			//portENABLE_INTERRUPTS();   //关闭5-15之间的中断
        //vTaskDelay(500);   
    }
}   

void led0_task_creat()
{
	 //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);  
}


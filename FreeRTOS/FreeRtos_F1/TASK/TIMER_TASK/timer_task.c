#include "timer_task.h"
#include "key.h"
#include "led.h"

/* ====================================== 软件定时器相关API函数 ======================================== */
/*
		xTimerCreate()                    创建定时器(默认不开启)                                              |
    xTimerCreateStatic()              静态创建定时器                                                      |       
 	  xTimerStart()	                    开启定时器                                                          |
    xTimerStartFromISR()              开启定时器(中断级)	                                              	|
		xTimerStop()                      停止定时器                                                          |
		xTimerStopFromISR()               停止定时器(中断级)                                                  |
		xTimerReset()                     复位定时器                                                          |
    xTimerResetFromISR		            复位定时器(中断级)                                                  |
*/
/* ===================================================================================================== */
TaskHandle_t TimerControlTask_Handler;
TimerHandle_t 	AutoReloadTimer_Handle;			//周期定时器句柄
TimerHandle_t	OneShotTimer_Handle;			//单次定时器句柄

void timercontrol_task(void *pvParameters)
{
	 u8 key;
	//只有两个定时器都创建成功了才能对其进行操作
	while(1)
	{
		if((AutoReloadTimer_Handle!=NULL)&&(OneShotTimer_Handle!=NULL))
		{
			key = KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRES:     //当key1按下的话打开两个定时器
					xTimerStart(AutoReloadTimer_Handle,0);	//开启周期定时器 第二个参数为阻塞时间
				  xTimerStart(OneShotTimer_Handle,0);		//开启单次定时器
					printf("开启定时器1和2\r\n");
					break;
				case KEY1_PRES:		//当key1按下话就关闭定时器
					xTimerStop(AutoReloadTimer_Handle,0); 	//关闭周期定时器
					xTimerStop(OneShotTimer_Handle,0); 		//关闭单次定时器
					printf("关闭定时器1和2\r\n");
					break;	
			}
		}
	}
}

void timercontrol_task_creat(void)
{
	//创建软件周期定时器
    AutoReloadTimer_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
									    (TickType_t			)1000,
							            (UBaseType_t		)pdTRUE,
							            (void*				)1,
							            (TimerCallbackFunction_t)AutoReloadCallback); //周期定时器，周期1s(1000个时钟节拍)，周期模式
    //创建单次定时器
	OneShotTimer_Handle=xTimerCreate((const char*			)"OneShotTimer",
							         (TickType_t			)2000,     //2000*1ms=2s
							         (UBaseType_t			)pdFALSE,  //单次定时器
							         (void*					)2,           //ID号
							         (TimerCallbackFunction_t)OneShotCallback); //单次定时器，周期2s(2000个时钟节拍)，单次模式		定时器回调函数			  
    //创建定时器控制任务
    xTaskCreate((TaskFunction_t )timercontrol_task,             
                (const char*    )"timercontrol_task",           
                (uint16_t       )TIMERCONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
                (TaskHandle_t*  )&TimerControlTask_Handler);    
}

//周期定时器的回调函数
void AutoReloadCallback(TimerHandle_t xTimer)
{
	LED0=!LED0;
}

//单次定时器的回调函数
void OneShotCallback(TimerHandle_t xTimer)
{
	LED1=!LED1;
  printf("定时器2运行结束\r\n");
}

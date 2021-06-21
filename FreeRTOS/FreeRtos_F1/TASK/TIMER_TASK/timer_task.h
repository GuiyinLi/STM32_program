#ifndef _timer_task_h
#define  _timer_task_h
#include "start_task.h"
#include "timers.h"


extern TimerHandle_t 	AutoReloadTimer_Handle;			//周期定时器句柄
extern TimerHandle_t	OneShotTimer_Handle;			//单次定时器句柄

void AutoReloadCallback(TimerHandle_t xTimer); 	//周期定时器回调函数
void OneShotCallback(TimerHandle_t xTimer);		//单次定时器回调函数

#endif




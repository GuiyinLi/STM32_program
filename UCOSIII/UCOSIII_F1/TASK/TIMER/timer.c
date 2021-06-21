#include "ucosiii_tmr.h"
#include "led.h"
#include "usart.h"
OS_TMR 	tmr1;		//定时器1
OS_TMR	tmr2;		//定时器2

void tmr1_task_create()
{
	OS_ERR err;
	//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
                (CPU_CHAR	*)"tmr1",	//定时器名字  OS_CFG_TMR_TASK_RATE_HZ=100 1/100=10ms->时间分辨率(>=5ms)
                (OS_TICK	 )20,			//20*10=200ms  初始化延迟时间 延迟之后 才进入定时器中断模式
                (OS_TICK	 )100,    //100*10=1000ms                  周期值
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
}

void tmr2_task_create()
{
	OS_ERR err;  //可通过OSTmrStart()来重新触发单次定时器
	//创建定时器2
	OSTmrCreate((OS_TMR		*)&tmr2,		//定时器1
                (CPU_CHAR	*)"tmr2",	//定时器名字  OS_CFG_TMR_TASK_RATE_HZ=100 1/100=10ms->时间分辨率(>=5ms)
                (OS_TICK	 )200,			//200*10=2000ms 
                (OS_TICK	 )0,   
                (OS_OPT		 )OS_OPT_TMR_ONE_SHOT, //单次定时器 没有周期
                (OS_TMR_CALLBACK_PTR)tmr2_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
}


/* 在回调函数中 应避免任何删除或堵塞定时任务的函数 */

/* =====================相关函数======================= */
/*                                                       |
    OSTmrCreate()	           //创建定时器                |
		OSTmrDel()	             //删除定时器                |
		OSTmrRemainGet()	       //获取定时器剩余时间        |
		OSTmrStart()	           //开启定时器                |
		OSTmrStateGet()	         //获取当前定时器状态        |
		OSTmrStop()	             //停止定时器                |
*/
/* ===================================================== */

//定时器1的回调函数
void tmr1_callback(void *p_tmr, void *p_arg)
{
	static u8 tmr1_num=0;
	tmr1_num++;		//定时器1执行次数加1
	LED1=!LED1;
}

//定时器2的回调函数
void tmr2_callback(void *p_tmr,void *p_arg)
{
	static u8 tmr2_num = 0;
	CPU_SR_ALLOC();        //为了进入临界区而写的代码  
	tmr2_num++;		//定时器2执行次数加1
	OS_CRITICAL_ENTER();	//进入临界区
	printf("定时器2运行结束\r\n");
	OS_CRITICAL_EXIT();		//退出临界区
}



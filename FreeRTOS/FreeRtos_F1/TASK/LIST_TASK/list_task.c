#include "list_task.h"
#include "key.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
//列表的遍历用于从多个同优先级的就绪任务中查找下一个要进行的任务

/* ====================== 列表&列表项 ====================== */
/*     
                             列表
														 uxNumberOfItems  列表项数目
														 pxIndex          当前指针
														 xListEnd
														 
                             列表项
	                         xItemValue
	                           pxNext
	                         pxPrevious
	                           pvOwner    所属控制块TCB
	                         pvContainer  所属列表
	                         
													    MINI
	                         xItemValue
	                           pxNext
	                         pxPrevious
*/
/* =========================================================== */									 
TaskHandle_t ListTask_Handler;

List_t TestList;		//测试用列表
ListItem_t ListItem1;	//测试用列表项1
ListItem_t ListItem2;	//测试用列表项2
ListItem_t ListItem3;

void list_task(void *pvParameters)
{
	//u16 list_task_num=0;
//	//第一步：初始化列表和列表项
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	
	ListItem1.xItemValue=40;			//ListItem1列表项值为40
	ListItem2.xItemValue=60;			//ListItem2列表项值为60
	ListItem3.xItemValue=50;			//ListItem3列表项值为50
	
	//第二步：打印列表和其他列表项的地址
	printf("/*******************列表和列表项地址*******************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList                          %#x					\r\n",(int)&TestList);
	printf("TestList->pxIndex                 %#x					\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd                %#x					\r\n",(int)(&TestList.xListEnd));
	printf("ListItem1                         %#x					\r\n",(int)&ListItem1);
	printf("ListItem2                         %#x					\r\n",(int)&ListItem2);
	printf("ListItem3                         %#x					\r\n",(int)&ListItem3);
	printf("/************************结束**************************/\r\n");
	printf("插入列表1\r\n");

	vListInsert(&TestList,&ListItem1);		//插入列表项ListItem1
	
	printf("/******************添加列表项ListItem1*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY0键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY0_PRES) delay_ms(10);					//等待KEY0键按下
	//第四步：向列表TestList添加列表项ListItem2，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem2);	//插入列表项ListItem2
	printf("/******************添加列表项ListItem2*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY0键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY0_PRES) delay_ms(10);					//等待KEY0键按下
	
	//第五步：向列表TestList添加列表项ListItem3，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem3);	//插入列表项ListItem3
	printf("/******************添加列表项ListItem3*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY1键删除!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY1_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第六步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	uxListRemove(&ListItem2);						//删除ListItem2
	printf("/******************删除列表项ListItem2*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY1键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=KEY1_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第七步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	TestList.pxIndex=TestList.pxIndex->pxNext;			//pxIndex向后移一项，这样pxIndex就会指向ListItem1。
	vListInsertEnd(&TestList,&ListItem2);				//列表末尾添加列表项ListItem2
	printf("/***************在末尾添加列表项ListItem2***************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->pxIndex                 %#x					\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************结束**************************/\r\n\r\n\r\n");
//	while(1)
//	{
//      list_task_num++;
//			taskENTER_CRITICAL();           //进入临界区->任务级   taskENTER_CRITICAL_From_ISR->中断级
//			printf("list_task run %d times\r\n",list_task_num);
//			taskEXIT_CRITICAL();            //退出临界区      taskEXIT_CRITICAL_From_ISR->中断级
//			delay_ms(10);
//	}
}

void list_task_creat(void)
{
	//创建LIST任务
    xTaskCreate((TaskFunction_t )list_task,     
                (const char*    )"list_task",   
                (uint16_t       )LIST_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&ListTask_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
}


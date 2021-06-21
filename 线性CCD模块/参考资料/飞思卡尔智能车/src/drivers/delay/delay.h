/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：delay.h
 * 描述         ：模拟延时函数头文件
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：野火嵌入式开发工作室
 * 淘宝店       ：http://firestm32.taobao.com
 * 技术支持论坛 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/


#ifndef __DELAY_H__
#define __DELAY_H__

#include "common.h"

void  delayms(uint32  ms);
void  delay_2us(void);//延时2us
void  delay_1us(void);//延时1us
void SamplingDelay(void);
void  delayus(void);

#endif
/******************** 唐野岐士 ********************
 * 文件名       ：FTM.c
 * 描述         ：FTM操作函数定义
 *
 * 实验平台     ：LQ K60开发板
 *               PWM占用FTM1的CH0、CH1、CH2、CH3通道。即PTC1、PTC2、PTC3、PTC4引脚
 *
 * 作者          DAQI
**********************************************************************************/

#include "include.h"


/*********** FTMn_CHn PWM输出初始化函数 ***********/
/*
        --FTM0--  --FTM1--  --FTM2--
CH0       PTC1      PTA8      PTA10
CH1       PTC2      PTA9      PTA11
CH2       PTC3       ×         ×
CH3       PTC4       ×         ×
CH4       PTD4       ×         ×
CH5       PTD5       ×         ×
CH6       PTD6       ×         ×
CH7       PTD7       ×         ×

×表示没有
*/


/*************************************************************************
*                   唐野岐士          
*
*  函数名称：FTM_PWM_init
*  功能说明：初始化FTM的PWM功能CH0、CH1、CH2、CH3通道。并设置频率为1ms
*  参数说明：所用端口引脚为 PTC1 PTC2 PTC3 PTC4
*  函数返回：无
*  修改时间：2013-4-3
*  备    注：同一个FTM，各通道的PWM频率是一样的
*************************************************************************/

void FTM_PWM_init(void)
{
   // SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;                          //使能PORTC时钟
    PORT_PCR_REG(PORTC_BASE_PTR, 1) = PORT_PCR_MUX(4);  // PTC1
    PORT_PCR_REG(PORTC_BASE_PTR, 2) = PORT_PCR_MUX(4);  // PTC2
    PORT_PCR_REG(PORTC_BASE_PTR, 3) = PORT_PCR_MUX(4);  // PTC3
    PORT_PCR_REG(PORTC_BASE_PTR, 4) = PORT_PCR_MUX(4);  // PTC4
    SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;                           //使能FTM0时钟
    FTM0_MODE|= FTM_MODE_WPDIS_MASK;// 写保护禁止
    //工作在左边沿对其PWM 模式MSB=1，ELSB:ELSA=10
    FTM0_C0SC |= FTM_CnSC_ELSB_MASK;//ELSB=1，ELSA=0，左对齐，先高后低
    FTM0_C0SC &= ~FTM_CnSC_ELSA_MASK;
    FTM0_C0SC |= FTM_CnSC_MSB_MASK;//MSB=1，模式选择边沿对齐
    FTM0_C1SC |= FTM_CnSC_ELSB_MASK;//ELSB=1，ELSA=0，左对齐，先高后低
    FTM0_C1SC &= ~FTM_CnSC_ELSA_MASK;
    FTM0_C1SC |= FTM_CnSC_MSB_MASK;//MSB=1，模式选择边沿对齐
    FTM0_C2SC |= FTM_CnSC_ELSB_MASK;//ELSB=1，ELSA=0，左对齐，先高后低
    FTM0_C2SC &= ~FTM_CnSC_ELSA_MASK;
    FTM0_C2SC |= FTM_CnSC_MSB_MASK;//MSB=1，模式选择边沿对齐
    FTM0_C3SC |= FTM_CnSC_ELSB_MASK;//ELSB=1，ELSA=0，左对齐，先高后低
    FTM0_C3SC &= ~FTM_CnSC_ELSA_MASK;
    FTM0_C3SC |= FTM_CnSC_MSB_MASK;//MSB=1，模式选择边沿对齐
    FTM0_SC=0x0c;//中断禁止,CLKS=01时钟源来自总线时钟48MHz，PS=100;16 分频后得到3MHz,CPWMS=0，递增计数
    FTM0_MODE&=~1;//FTM0 使能 
    FTM0_OUTMASK=0Xf0;// 通道CH0、CH1、CH2、CH3 正常输出，屏蔽其它通道
    FTM0_QDCTRL&=~FTM_QDCTRL_QUADEN_MASK;// 禁止正交解码模式
    FTM0_COMBINE=0;//DECAPEN=0 ，双边沿捕捉禁止， COMBINE=0 ，不级联
    FTM0_CNTIN=0;//FTM0 计数器初始值为 0
    FTM0_MOD=599;// 结束值，周期为（ MOD-CNTIN+1 ） * 时钟周期 =0.4ms 
    FTM0_C0V=0;//高电平0ms
    FTM0_C1V=0;//高电平0ms
    FTM0_C2V=0;//高电平0ms
    FTM0_C3V=0;//高电平0ms
    FTM0_CNT=100;                                               
}

/*************************************************************************
*                   唐野岐士          
*  功能说明：占空比控制、void L_PWM_GO(uint8 duty)；其中duty  0-1000;数值越大，速度越大
*  参数说明：所用端口引脚为 PTC1 PTC2 PTC3 PTC4
*  函数返回：无
*  修改时间：2013-4-3
*  备    注：同一个FTM，各通道的PWM频率是一样的
*************************************************************************/

void R_PWM_BACK(uint16 duty)
{
  uint16 speed;
  speed=duty*3;
  FTM0_C2V=speed;
  
}
void R_PWM_GO(uint16 duty)
{
  uint16 speed;
  speed=duty*3;
  FTM0_C3V=speed;
}
void L_PWM_GO(uint16 duty)
{
  uint16 speed;
  speed=duty*3;
  FTM0_C1V=speed;

}
void L_PWM_BACK(uint16 duty)
{
  uint16 speed;
  speed=duty*3;
  FTM0_C0V=speed;

}

/*************************************************************************
*                   唐野岐士          
*
*  函数名称：FTM_QUAD_init
*  功能说明：正交解码
*  参数说明：PTA10 引脚为FTM2_PHA 功能;PTA11 引脚为FTM2_PHB 功能  用于左轮编码器
             PTA8 引脚为FTM1_PHA 功能; PTA9 引脚为FTM1_PHB 功能  用于右轮编码器

*  如果计数溢出，可能为正向溢出，也可能为反向溢出。正向溢出时，计数器累加计数到
MOD 后，即返回到CNTIN，这时TOF 置位，而TOFDIR 位也置1，表示正向溢出。反向溢出
时，即计数器递减计数，从CNTIN 变为MOD 时，TOF 置1，但TOFDIR 为0，表示反向溢出。
*

*  修改时间：2013-4-6
*  备    注：同一个FTM，各通道的PWM频率是一样的
*************************************************************************/
void FTM_QUAD_init(void)
{
//  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;                          //使能PORTA时钟
  PORTA_PCR10= PORT_PCR_MUX(6); // 设置引脚PTA10 引脚为FTM2_PHA 功能
  PORTA_PCR11= PORT_PCR_MUX(6); // 设置引脚PTA11 引脚为FTM2_PHB 功能
  SIM_SCGC3|=SIM_SCGC3_FTM2_MASK;//使能FTM2 时钟
  FTM2_MODE|= FTM_MODE_WPDIS_MASK;// 写保护禁止
  FTM2_QDCTRL|=FTM_QDCTRL_QUADMODE_MASK;//AB 相同时确定方向和计数值
  FTM2_CNTIN=0;//FTM0 计数器初始值为 0
  FTM2_MOD=65535;// 结束值
  FTM2_QDCTRL|=FTM_QDCTRL_QUADEN_MASK;// 启用 FTM2 正交解码模式
  FTM2_MODE |= FTM_MODE_FTMEN_MASK;//FTM2EN =1
  FTM2_CNT=0;
  
  PORTA_PCR8= PORT_PCR_MUX(6); // 设置引脚PTA10 引脚为FTM2_PHA 功能
  PORTA_PCR9= PORT_PCR_MUX(6); // 设置引脚PTA11 引脚为FTM2_PHB 功能
  SIM_SCGC6|=SIM_SCGC6_FTM1_MASK;//使能FTM1 时钟
  FTM1_MODE|= FTM_MODE_WPDIS_MASK;// 写保护禁止
  FTM1_QDCTRL|=FTM_QDCTRL_QUADMODE_MASK;//AB 相同时确定方向和计数值
  FTM1_CNTIN=0;//FTM0 计数器初始值为 0
  FTM1_MOD=65535;// 结束值
  FTM1_QDCTRL|=FTM_QDCTRL_QUADEN_MASK;// 启用 FTM  正交解码模式
  FTM1_MODE |= FTM_MODE_FTMEN_MASK;//FTM1EN =1
  FTM1_CNT=0;
  
}
/******************** 唐野岐士 ********************
 * 文件名       ：adc.c
 * 描述         ：ad操作函数定义
 *
 * 实验平台     ：LQ K60开发板
 *                ang PTB11  Gyto PTB10  Z PTE24
 *
 * 作者          DAQI
**********************************************************************************/

#include "include.h"


tADC_Config Master_Adc_Config;          //该结构体包含了需要的ADC/PGA配置

volatile struct ADC_MemMap *ADCx[2] = {ADC0_BASE_PTR, ADC1_BASE_PTR}; //定义两个指针数组保存 ADCx 的地址

/******************** 唐野岐士 ********************
*
*  函数名称：adc_init
*  功能说明：AD初始化，使能时钟,初始化PTE24 PTB10 PTB11为ad功能

*  修改时间：2013-4-7
*************************************************************************/
volatile uint32 Gyro_sum,Gravity_sum;
volatile uint16 Gyro_,Gravity_;
void adc_init(void)
{
    SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );        //开启ADC0时钟
    SIM_SOPT7 &= ~(SIM_SOPT7_ADC0ALTTRGEN_MASK  | SIM_SOPT7_ADC0PRETRGSEL_MASK);
    SIM_SOPT7 = SIM_SOPT7_ADC0TRGSEL(0);
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORT_PCR_REG(PORTE_BASE_PTR, 24) =  PORT_PCR_MUX(0);    //初始化PTE24为ad功能
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORT_PCR_REG(PORTB_BASE_PTR, 2) =  PORT_PCR_MUX(0);    //初始化 PTB2为ad功能
    
    SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK );
    SIM_SOPT7 &= ~(SIM_SOPT7_ADC1ALTTRGEN_MASK  | SIM_SOPT7_ADC1PRETRGSEL_MASK) ;
    SIM_SOPT7 = SIM_SOPT7_ADC1TRGSEL(0);
    
//    PORT_PCR_REG(PORTB_BASE_PTR, 10) =  PORT_PCR_MUX(0);
    PORT_PCR_REG(PORTB_BASE_PTR, 11) =  PORT_PCR_MUX(0);    //初始化 PTB11为ad功能

   
}
uint16 CCD_value(void)     //集三次一路模拟量的AD值，返回 中值
{
    u16  temp;
    
    temp=ad_once(ADC1, AD15, ADC_12bit);
    return (temp);
}
uint16 Gyro_value(void)     
{
//  u16 Gyro[20];

  Gyro_sum=0;
  uint8 i;
  for(i=0;i<20;i++)
  {
    Gyro_=ad_once(ADC0, AD12, ADC_16bit);
    Gyro_sum+=Gyro_;
  }//20次转换取平均值，低通滤波
  Gyro_= Gyro_sum/20;
   
    return (Gyro_);
} 
uint16 Gravity_value(void)     
{
 // u16 Gravity[20];
//  u16 Gravity_;
  
  Gravity_sum=0;
//  Gyro_sum=0；
  uint8 i;
  for(i=0;i<20;i++)
  {
    Gravity_=ad_once(ADC0, AD17, ADC_12bit);
    Gravity_sum+=Gravity_;
  }//20次转换取平均值，低通滤波
  Gravity_= Gravity_sum/20;
   
    return (Gravity_);
} 
/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：ad_once
*  功能说明：采集一次一路模拟量的AD值
*  参数说明：ADCn        模块号（ ADC0、 ADC1）
*            ADC_Channel 通道号
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函数返回：无符号结果值
*  修改时间：2012-2-10
*  备    注：参考苏州大学的例程，B通道不能软件触发！！！！
*************************************************************************/
u16 ad_once(ADCn adcn, ADC_Ch ch, ADC_nbit bit) //采集某路模拟量的AD值
{
    u16 result = 0;

    adc_start(adcn, ch, bit);	  //启动ADC转换

    while (( ADC_SC1_REG(ADCx[adcn], 0 ) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK);
    result = ADC_R_REG(ADCx[adcn], 0);
    ADC_SC1_REG(ADCx[adcn], 0) &= ~ADC_SC1_COCO_MASK;
    return result;
}

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：adc_start
*  功能说明：启动adc软件采样，B通道不能用于软件触发！！！！
*  参数说明：ADCx        模块号（ ADC0、 ADC1）
*            ADC_Channel 通道号
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函数返回：无
*  修改时间：2012-2-10
*  备    注：修改苏州大学的例程
*************************************************************************/
void adc_start(ADCn adcn, ADC_Ch ch, ADC_nbit bit)
{

    Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH( ch );

    //初始化ADC默认配置
    Master_Adc_Config.CONFIG1  = ADLPC_NORMAL
                                 | ADC_CFG1_ADIV(ADIV_4)
                                 | ADLSMP_LONG
                                 | ADC_CFG1_MODE(bit)
                                 | ADC_CFG1_ADICLK(ADICLK_BUS);
    Master_Adc_Config.CONFIG2  = MUXSEL_ADCA    //MUXSEL_ADCA
                                 | ADACKEN_DISABLED
                                 | ADHSC_HISPEED
                                 | ADC_CFG2_ADLSTS(ADLSTS_20) ;

    Master_Adc_Config.COMPARE1 = 0x1234u ;                 //任意值
    Master_Adc_Config.COMPARE2 = 0x5678u ;                 //任意值

    adc_config_alt(ADCx[adcn], &Master_Adc_Config);       // 配置 ADCn
}

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：adc_stop
*  功能说明：停止ADC转换
*  参数说明：ADCx        模块号（ ADC0、 ADC1）
*            ADC_Channel 通道号
*  函数返回：无
*  修改时间：2012-2-10
*  备    注：修改苏州大学的例程
*************************************************************************/
void adc_stop(ADCn adcn)
{
    Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(Module_disabled);
    adc_config_alt(ADCx[adcn], &Master_Adc_Config);  // 配置ADC0
}


/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：adc_config_alt
*  功能说明：将adc寄存器结构体配置进adc寄存器
*  参数说明：adcmap      adc基址寄存器地址（ADC0_BASE_PTR,ADC1_BASE_PTR）
*            ADC_CfgPtr  存放 寄存器值的结构体
*  函数返回：无
*  修改时间：2012-2-10
*  备    注：修改官方工程的例程
*************************************************************************/
void adc_config_alt(ADC_MemMapPtr adcmap, tADC_ConfigPtr ADC_CfgPtr)
{
    ADC_CFG1_REG(adcmap) = ADC_CfgPtr->CONFIG1;
    ADC_CFG2_REG(adcmap) = ADC_CfgPtr->CONFIG2;
    ADC_CV1_REG(adcmap)  = ADC_CfgPtr->COMPARE1;
    ADC_CV2_REG(adcmap)  = ADC_CfgPtr->COMPARE2;
    ADC_SC2_REG(adcmap)  = ADC_CfgPtr->STATUS2;
    ADC_SC3_REG(adcmap)  = ADC_CfgPtr->STATUS3;
    ADC_PGA_REG(adcmap)  = ADC_CfgPtr->PGA;
    ADC_SC1_REG(adcmap, A) = ADC_CfgPtr->STATUS1A;
    ADC_SC1_REG(adcmap, B) = ADC_CfgPtr->STATUS1B;
}




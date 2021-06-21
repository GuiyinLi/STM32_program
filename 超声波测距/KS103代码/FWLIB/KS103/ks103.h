#ifndef _ks103_h
#define _ks103_h
#include "sys.h"

/*定义命令字节*/
#define Notemp0_5m     0xb0     //无温度补偿 返回距离(mm)0-5m   耗时32ms
#define Temp0_5m       0xb4     //有温度补偿                        87ms
#define Notemp0_11m    0xb8     //                                  68ms
#define Temp0_11m      0xbc     //                                  87ms
/*定义器件地址*/
#define SlaveAddress1   0xE8     //默认器件地址
#define SlaveAddress2   0xd0
#define SlaveAddress3   0xd2
#define SlaveAddress4   0xd4
#define SlaveAddress5   0xd6
#define SlaveAddress6   0xd8
#define SlaveAddress7   0xda
#define SlaveAddress8   0xdc
#define SlaveAddress9   0xde
#define SlaveAddress10  0xe0
#define SlaveAddress11  0xe2
#define SlaveAddress12  0xe4
#define SlaveAddress13  0xe6
#define SlaveAddress14  0xea
#define SlaveAddress15  0xec
#define SlaveAddress16  0xee
#define SlaveAddress17  0xf8
#define SlaveAddress18  0xfa
#define SlaveAddress19  0xfc
#define SlaveAddress20  0xfe
/**降噪指令 第一至六级降噪**************/
#define Noise_Reduction_1 0x70      //默认    
#define Noise_Reduction_2 0x71      //适用于电脑USB供电降噪
#define Noise_Reduction_3 0x72      //使用较长距离的USB供电
#define Noise_Reduction_4 0x73      //适用于开关电源供电
#define Noise_Reduction_5 0x74      //适用于噪音较大的开关电源供电
#define Noise_Reduction_6 0x75      //适用于高噪音电源供电
/*寄存器*/
#define Res_2 0x02
#define Res_3 0x03
//MODE(NC)引脚悬空时为IIC模式 接地时为串口模式
void Ks103_Init(void);
void Ks103_Write_byte(u8 SlaveAddress,u8 ResAddress,u8 command);
u8 Ks103_Read_byte(u8 SlaveAddress,u8 ResAddress);
void Change_Addr(u8 OldAdddr,u8 NewAddr);
u32 Read_Ks103_Data(u8 SlaveAddress,u8 command);
float Get_Data(u32 data,u8 n);
void Send_Command(u8 command);




#endif





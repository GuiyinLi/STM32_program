#ifndef __IMGPROCESS_H
#define __IMGPROCESS_H 

#include"include.h"
#define imgshow 0 //0代表只使用图像调试功能（仅仅靠图像，调试数据），1代表正常运行（带PID的跑车）
#if imgshow
extern u8 sobel(u8 (*img)[CAMERA_W],u16 tresh,u16 *center,u16 *set,u8 *type);//sobel老算法
extern u8 type_detect(u8 (*img)[CAMERA_W],u16 tresh,u16 *center,u16 *set,u8 *type);//跳沿新算法
#else 
extern u8 sobel(u8 (*img)[CAMERA_W],u8 (*imgout)[CAMERA_W],u16 tresh);//sobel老算法
extern u8 outline(u8 (*img)[CAMERA_W],u8 (*imgout)[CAMERA_W],u16 tresh);//跳沿新算法
#endif
typedef enum 
{
    RUNWAY_NOTINIT=0,
	RUNWAY_STRAIGHT,			//直道
	RUNWAY_TURNLEFT,				//左转
	RUNWAY_TURNRIGHT,				//右转
	RUNWAYSTOPLINE,				//停止线
	RUNWAY_CROSS,				//十字路口
	
}RUNWAY_STATE;

#endif

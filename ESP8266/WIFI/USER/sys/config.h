#ifndef  _CONFIG_H
#define  _CONFIG_H

#define INFANTRY                     3       // 步兵编号  1 步兵一 2 步兵二 3 步兵三 4 步兵四 
#define DEBUG_MODE                   0       //1 启动调试模式（只开启摩擦轮与拨弹电机）   0 不启动调试模式 
#define DIALPOSITIONORSPEED          1        //1 拨弹电机位置环   0拨弹电机速度环
#define CHASSIS_FEEDBACK             2        //1启用裁判系 统    2 启用电流计  0 不启用功率闭环
#define SHOOT_FEEDBACK               0        //1启用裁判系统(热量与等级)    2 定时器计算  3裁判系统(只是读取等级)  0 不启用热量闭环

#define ACCELERA_TIME               52000     //加速斜坡时间

//#define SONEBULLET_DISTANCE         1800    //一颗子弹的射频距离敏感度(一级)
//#define MONEBULLET_DISTANCE         2600    //一颗子弹的射频距离敏感度(二级)
//#define LONEBULLET_DISTANCE         3500    //一颗子弹的射频距离敏感度(三级)

//#define STHREEBULLET_DISTANCE       1500    //三颗子弹的射频距离敏感度(一级)
//#define MTHREEBULLET_DISTANCE       2200    //三颗子弹的射频距离敏感度(二级)
//#define LTHREEBULLET_DISTANCE       3000    //三颗子弹的射频距离敏感度(三级)


#define SONEBULLET_DISTANCE         1200    //一颗子弹的射频距离敏感度(一级)
#define MONEBULLET_DISTANCE         1800    //一颗子弹的射频距离敏感度(二级)
#define LONEBULLET_DISTANCE         2700    //一颗子弹的射频距离敏感度(三级)

#define STHREEBULLET_DISTANCE       800    //三颗子弹的射频距离敏感度(一级)
#define MTHREEBULLET_DISTANCE       1100    //三颗子弹的射频距离敏感度(二级)
#define LTHREEBULLET_DISTANCE       1600    //三颗子弹的射频距离敏感度(三级)



//#define SONEBULLET_DISTANCE         800    //一颗子弹的射频距离敏感度(一级)
//#define MONEBULLET_DISTANCE         1400    //一颗子弹的射频距离敏感度(二级)
//#define LONEBULLET_DISTANCE         2200    //一颗子弹的射频距离敏感度(三级)

//#define STHREEBULLET_DISTANCE       600    //三颗子弹的射频距离敏感度(一级)
//#define MTHREEBULLET_DISTANCE       1100    //三颗子弹的射频距离敏感度(二级)
//#define LTHREEBULLET_DISTANCE       1700    //三颗子弹的射频距离敏感度(三级)
//#define SONEBULLET_DISTANCE         2100    //一颗子弹的射频距离敏感度(一级)
//#define MONEBULLET_DISTANCE         2800    //一颗子弹的射频距离敏感度(二级)
//#define LONEBULLET_DISTANCE         3800    //一颗子弹的射频距离敏感度(三级)

//#define STHREEBULLET_DISTANCE       1800    //三颗子弹的射频距离敏感度(一级)
//#define MTHREEBULLET_DISTANCE       2300    //三颗子弹的射频距离敏感度(二级)
//#define LTHREEBULLET_DISTANCE       3100    //三颗子弹的射频距离敏感度(三级)
#if INFANTRY==1
#define POWER_KP                   1.5f     //功率环KP值
#define POWER_KI                   0.32f     //功率环KI值增大可减缓超功率
#define POWER_KD                   0.15f     //功率环KD值
//#define POWER_KP                   1.5f     //功率环KP值
//#define POWER_KI                   0.3f     //功率环KI值
//#define POWER_KD                   0.3f     //功率环KD值
//#define POWER_KD                   0.12f     //功率环KD值
#elif INFANTRY==2
#define POWER_KP                   1.5f     //功率环KP值
#define POWER_KI                   0.3f     //功率环KI值增大可减缓超功率
#define POWER_KD                   0.12f    //功率环KD值
#elif INFANTRY==3
#define POWER_KP                   1.5f     //功率环KP值
#define POWER_KI                   0.32f     //功率环KI值增大可减缓超功率
#define POWER_KD                   0.15f    //功率环KD值
#endif
#define PTZ_LOCK_ANGLE               2.6f    //云台锁死角度
#define REDIAL_BULLET_SPEED          3       //拨弹电机的速度检测速度
#define KEY_PRESS_DELAY              30    //按键响应延时时间
#define FRICITION_CLOSE_DELAY        75      //摩擦轮关闭延时时间
#define TWIST_DELAY                  160     //扭腰频率（谨慎调节）
#define TWIST_ANGLE                  35      //扭腰角度（谨慎调节）
#define AVOID_ANGLE                  35      //躲闪角度（谨慎调节）


#define FASTSPEED_OFFSET             0.78f   //急速超功率系数
#define HIGHSPEED_OFFSET             0.85f  //高速超功率系数
#define LOWSPEED_OFFSET              0.9f   //低速超功率系数

#define ONE_KP                       1.2f   //一颗子弹位置环KP   1.5F
#define ONE_KD                       0.18f    //一颗子弹位置环KD
#define THREE_KP                     0.55f    //三颗子弹位置环KP
#define THREE_KD                     0        //三颗子弹位置环KD

#define POWER_VOLTAGE                24.0f   //电源电压
#define POWER_LIMIT_UP               18     //输出功率上限  //100
#define CURRENT_OFFSET               1000.0f //电流比例系数
#if INFANTRY == 1               //步兵一参数↓


/**********************   子弹位置与热量修改↓  ***********************/
		#define      SLOW_ONEBULLET_HEAT          18   //一颗低速子弹的热量    
		#define   	 FAST_ONEBULLET_HEAT          30   //一颗高速子弹的热量  

		#define 		 SLOW_THREEBULLET_HEAT        54   //三颗低速子弹的热量   
		#define 		 FAST_THREEBULLET_HEAT        85   //三颗高速子弹的热量  

		#define      DIAL_ONE_POSITION       	    5040 //一颗子弹的位置
    #define      DIAL_THREE_POSITION     	    15150//三颗子弹的位置
		#define      DIAL_BACK_POSITION         	2000 //倒弹的位置
#if		DIALPOSITIONORSPEED==1
		#define      LOW_ONE_TIME                30000//一颗子弹的时间（低速 20m/s）
		#define      FAST_ONE_TIME               15000//一颗子弹的时间（高速 17m/s）	
		#define      LOW_THREE_TIME              105000//三颗子弹的时间（低速 20m/s）
		#define      FAST_THREE_TIME             45000//三颗子弹的时间（高速 17m/s ）		
#else		
		#define      LOW_ONE_TIME                35000 //一颗子弹的时间（低速 20m/s ）
		#define      FAST_ONE_TIME               19000 //一颗子弹的时间（高速 17m/s ）
		#define      LOW_THREE_TIME              120000//三颗子弹的时间（低速 20m/s ）
		#define      FAST_THREE_TIME             58000 //三颗子弹的时间（高速 17m/s）
		#define      DIAL_SPEED                  4150 //速度环拨弹速度
		#define      DIAL_ONE_DELAY               11  //一颗子弹的延时
		#define      DIAL_THREE_DELAY             40  //三颗子弹的延时
#endif			
		#define      NORMAL_FRICITION_ON_SPEED   1700   //普通摩擦轮基础速度 （Vave=25m/s）
    #define      LOW_FRICITION_ON_SPEED      1380   //大火力摩擦轮基础速度(Vave=18/s)		
/***********************    功率环参数修改↓   ************************/		
		#define      POWER_LIMIT_VALUE      	 76       //高速模式下的功率
		#define      KEY_LOWSPEED_KP     	  NORMAL_ZHI_LOWSPEED_KP//低速模式的KP
    #define      KEY_HIGHSPEED_KP     	NORMAL_ZHI_HIGHSPEED_KP//高速模式的KP
		#define      FRICITION_ON_SPEED   	NORMAL_FRICITION_ON_SPEED//摩擦轮开启速度
		
/***********************    云台参数修改↓   ************************/				
		#define      PITCH_INITIAL_VALUE     5080    //Pitch轴初始值
		#define      YAW_INITIAL_VALUE       6170    //Yaw轴初始值
		#define      PITCH_UP_LIMIT          14.5f   //pitch上限
		#define      PITCH_DOWM_LIMIT        -7.5f  //pitch下限
/***********************   陀螺仪参数修改↓   ************************/		
    #define      AXSET                   -5    //加速度计x轴
		#define      AYSET                   6  //加速度计y轴
		#define      AZSET                   159    //加速度计z轴
		#define      GXSET                   -8    //陀螺仪x轴
		#define      GYSET                   1     //陀螺仪y轴
		#define      GZSET                   -7    //陀螺仪z轴

#endif

#if INFANTRY == 2               //步兵二参数↓

		#define      SLOW_ONEBULLET_HEAT          19   //一颗低速子弹的热量    
		#define   	 FAST_ONEBULLET_HEAT          27   //一颗高速子弹的热量  

		#define 		 SLOW_THREEBULLET_HEAT        57   //三颗低速子弹的热量   
		#define 		 FAST_THREEBULLET_HEAT        81   //三颗高速子弹的热量   

	  #define      DIAL_ONE_POSITION       5040 //一颗子弹的位置
    #define      DIAL_THREE_POSITION     15150//三颗子弹的位置
		#define      DIAL_BACK_POSITION      2000 //倒弹的位置
		
#if		DIALPOSITIONORSPEED==1
		#define      LOW_ONE_TIME                30000//一颗子弹的时间（低速 20m/s）
		#define      FAST_ONE_TIME               15000//一颗子弹的时间（高速 17m/s）	
		#define      LOW_THREE_TIME              105000//三颗子弹的时间（低速 20m/s）
		#define      FAST_THREE_TIME             45000//三颗子弹的时间（高速 17m/s ）		
#else		
		#define      LOW_ONE_TIME                35000 //一颗子弹的时间（低速 20m/s ）
		#define      FAST_ONE_TIME               19000 //一颗子弹的时间（高速 17m/s ）
		#define      LOW_THREE_TIME              120000//三颗子弹的时间（低速 20m/s ）
		#define      FAST_THREE_TIME             58000 //三颗子弹的时间（高速 17m/s）
		#define      DIAL_SPEED                  4150 //速度环拨弹速度
		#define      DIAL_ONE_DELAY               11  //一颗子弹的延时
		#define      DIAL_THREE_DELAY             40  //三颗子弹的延时
#endif			
		
		#define      NORMAL_FRICITION_ON_SPEED   1820   //普通摩擦轮基础速度 （Vave=25m/s）
    #define      LOW_FRICITION_ON_SPEED      1440   //大火力摩擦轮基础速度(Vave=18/s)				
/***********************    功率环参数修改↓   ************************/		
		#define      POWER_LIMIT_VALUE      	 76       //高速模式下的功率
		#define      KEY_LOWSPEED_KP       NORMAL_ZHI_LOWSPEED_KP//低速模式的KP
    #define      KEY_HIGHSPEED_KP      NORMAL_ZHI_HIGHSPEED_KP//高速模式的KP
		#define      FRICITION_ON_SPEED    NORMAL_FRICITION_ON_SPEED//摩擦轮开启速度
/***********************    云台参数修改↓   ************************/				
		#define      PITCH_UP_LIMIT          30  //pitch上限
		#define      PITCH_DOWM_LIMIT        -10   //pitch下限
		#define      PITCH_INITIAL_VALUE     1200 //Pitch轴初始值  2773   越小越向上
		#define      YAW_INITIAL_VALUE       3985 //Yaw轴初始值3985
/***********************   陀螺仪参数修改↓   ************************/	
    #define      AXSET                   130    //加速度计x轴
		#define      AYSET                   0  //加速度计y轴
		#define      AZSET                   250    //加速度计z轴
		#define      GXSET                   -2    //陀螺仪x轴
		#define      GYSET                   15     //陀螺仪y轴
		#define      GZSET                   -24    //陀螺仪z轴

#endif


#if INFANTRY == 3               //步兵三参数↓
/**********************   子弹位置与热量修改↓  ***********************/

		#define      SLOW_ONEBULLET_HEAT          19   //一颗低速子弹的热量    
		#define   	 FAST_ONEBULLET_HEAT          29   //一颗高速子弹的热量  

		#define 		 SLOW_THREEBULLET_HEAT        57   //三颗低速子弹的热量   
		#define 		 FAST_THREEBULLET_HEAT        84   //三颗高速子弹的热量  

	  #define      DIAL_ONE_POSITION       5040 //一颗子弹的位置
    #define      DIAL_THREE_POSITION     15150//三颗子弹的位置
		#define      DIAL_BACK_POSITION      2000 //倒弹的位置

#if		DIALPOSITIONORSPEED==1
		#define      LOW_ONE_TIME                30000//一颗子弹的时间（低速 20m/s）
		#define      FAST_ONE_TIME               15000//一颗子弹的时间（高速 17m/s）	
		#define      LOW_THREE_TIME              105000//三颗子弹的时间（低速 20m/s）
		#define      FAST_THREE_TIME             45000//三颗子弹的时间（高速 17m/s ）		
#else		
		#define      LOW_ONE_TIME                35000 //一颗子弹的时间（低速 20m/s ）
		#define      FAST_ONE_TIME               19000 //一颗子弹的时间（高速 17m/s ）
		#define      LOW_THREE_TIME              120000//三颗子弹的时间（低速 20m/s ）
		#define      FAST_THREE_TIME             58000 //三颗子弹的时间（高速 17m/s）
		#define      DIAL_SPEED                  4150 //速度环拨弹速度
		#define      DIAL_ONE_DELAY               11  //一颗子弹的延时
		#define      DIAL_THREE_DELAY             40  //三颗子弹的延时
#endif			
		
		#define      NORMAL_FRICITION_ON_SPEED   1430   //普通摩擦轮基础速度 （Vave=27m/s）
    #define      LOW_FRICITION_ON_SPEED      1225   //大火力摩擦轮基础速度(Vave=18/s)	
/***********************    功率环参数修改↓   ************************/		

		#define      POWER_LIMIT_VALUE      	 17       //高速模式下的功率
		#define      KEY_LOWSPEED_KP         NORMAL_ZHI_LOWSPEED_KP//低速模式的KP
    #define      KEY_HIGHSPEED_KP        NORMAL_ZHI_HIGHSPEED_KP//高速模式的KP
		#define      FRICITION_ON_SPEED      NORMAL_FRICITION_ON_SPEED //摩擦轮开启速度
/***********************    云台参数修改↓   ************************/				
		#define      PITCH_UP_LIMIT          16.5f   //pitch上限
		#define      PITCH_DOWM_LIMIT        -11.5f   //pitch下限
		#define      PITCH_INITIAL_VALUE     6500 //Pitch轴初始值
		#define      YAW_INITIAL_VALUE       646 //Yaw轴初始值
/***********************   陀螺仪参数修改↓   ************************/	  //陀螺仪的初始偏差
    #define      AXSET                   -114    //加速度计x轴
		#define      AYSET                   177  //加速度计y轴
		#define      AZSET                   27    //加速度计z轴
		#define      GXSET                   14    //陀螺仪x轴
		#define      GYSET                   13     //陀螺仪y轴
		#define      GZSET                   -14   //陀螺仪z轴

#endif

#if INFANTRY == 4                //步兵四参数↓
/**********************   子弹位置与热量修改↓  ***********************/

		#define      SLOW_ONEBULLET_HEAT          18   //一颗低速子弹的热量    
		#define   	 FAST_ONEBULLET_HEAT          21   //一颗高速子弹的热量  

		#define 		 SLOW_THREEBULLET_HEAT        54   //三颗低速子弹的热量   
		#define 		 FAST_THREEBULLET_HEAT        63   //三颗高速子弹的热量    

	  #define      DIAL_ONE_POSITION       5040 //一颗子弹的位置
    #define      DIAL_THREE_POSITION     15150//三颗子弹的位置
		#define      DIAL_BACK_POSITION      2000 //倒弹的位置

#if		DIALPOSITIONORSPEED==1
		#define      LOW_ONE_TIME                30000//一颗子弹的时间（低速 20m/s）
		#define      FAST_ONE_TIME               15000//一颗子弹的时间（高速 17m/s）	
		#define      LOW_THREE_TIME              105000//三颗子弹的时间（低速 20m/s）
		#define      FAST_THREE_TIME             45000//三颗子弹的时间（高速 17m/s ）		
#else		
		#define      LOW_ONE_TIME                35000 //一颗子弹的时间（低速 20m/s ）
		#define      FAST_ONE_TIME               19000 //一颗子弹的时间（高速 17m/s ）
		#define      LOW_THREE_TIME              120000//三颗子弹的时间（低速 20m/s ）
		#define      FAST_THREE_TIME             58000 //三颗子弹的时间（高速 17m/s）
		#define      DIAL_SPEED                  4150 //速度环拨弹速度
		#define      DIAL_ONE_DELAY               11  //一颗子弹的延时
		#define      DIAL_THREE_DELAY             40  //三颗子弹的延时
#endif			
		#define      NORMAL_FRICITION_ON_SPEED   1600   //普通摩擦轮基础速度 （Vave=21m/s）
    #define      LOW_FRICITION_ON_SPEED      1240   //大火力摩擦轮基础速度(Vave=18/s)	
/***********************    功率环参数修改↓   ************************/		
		#define      POWER_LIMIT_VALUE      	 76       //高速模式下的功率
		#define      KEY_LOWSPEED_KP         NORMAL_ZHI_LOWSPEED_KP//低速模式的KP
    #define      KEY_HIGHSPEED_KP        NORMAL_ZHI_HIGHSPEED_KP//高速模式的KP
		#define      FRICITION_ON_SPEED      NORMAL_FRICITION_ON_SPEED //摩擦轮开启速度
/***********************    云台参数修改↓   ************************/				
		#define      PITCH_UP_LIMIT          13   //pitch上限
		#define      PITCH_DOWM_LIMIT        -3.5f   //pitch下限
		#define      PITCH_INITIAL_VALUE     6500 //Pitch轴初始值
		#define      YAW_INITIAL_VALUE       2680 //Yaw轴初始值
/***********************   陀螺仪参数修改↓   ************************/	
    #define      AXSET                   -114    //加速度计x轴
		#define      AYSET                   177  //加速度计y轴
		#define      AZSET                   27    //加速度计z轴
		#define      GXSET                   14    //陀螺仪x轴
		#define      GYSET                   13     //陀螺仪y轴
		#define      GZSET                   -14   //陀螺仪z轴

		
#endif





#endif






#ifndef     _ANGLE_H_
#define     _ANGLE_H_ 1u

#define TSL_SI_High  GPIO_SET(PORTC,18,1)
#define TSL_SI_Low  GPIO_SET(PORTC,18,0)
#define TSL_CLK_High  GPIO_SET(PORTC,19,1)
#define TSL_CLK_Low  GPIO_SET(PORTC,19,0)

void Kalman_Filter(void);
void AngleControl(void);
void MotorSpeedOut(void);
void Get_Speed(void);
void SpeedControlOutput(void);
void CCD_Init(void);
void Get_Loction(void);
void StartIntegration(void);   //ÆØ¹âº¯Êý
void Loction_Filter(void);
void DirectionControlOutput(void);
void CarControl(void);
void convery_ccd1(void);
void convery_ccd0(void);
void convery_ccd2(void);
void convery_ccd3(void);

#endif  //_ANGLE_H_


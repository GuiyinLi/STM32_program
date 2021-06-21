#include "common.h"
#include  "include.h"
#include  "MPU.h"

void MPU_init()
{
       /*MPU6950≥ı ºªØ*/
        I2C_init(I2C1);
        I2C_WriteAddr(I2C1, SlaveAddress, PWR_MGMT_1, 0x00); 
        I2C_WriteAddr(I2C1, SlaveAddress, SMPLRT_DIV, 0x07); 
        I2C_WriteAddr(I2C1, SlaveAddress, CONFIG, 0x06); 
        I2C_WriteAddr(I2C1, SlaveAddress, GYRO_CONFIG, 0x18); 
        I2C_WriteAddr(I2C1, SlaveAddress, ACCEL_CONFIG, 0x01); 

}

double z_anglespeed_get()
{
   u16 data[2];
   int z_data;
   long int z_sum;
   double z_aver,z_anglespeed;
   double FS= 0.060976;
   data[0]=I2C_ReadAddr(I2C1, SlaveAddress, GYRO_ZOUT_L);
   
   
   
   
     data[1]=I2C_ReadAddr(I2C1, SlaveAddress, GYRO_ZOUT_H);
   
   
   return (data[0]);


}
  u16 data[2];
  int y_data;
  long int y_sum;
  double y_aver,y_anglespeed;
  double FS= 0.060976;

double yy_anglespeed()
{
         for(int n=0;n<3;n++)
          {   
           
             data[1]=I2C_ReadAddr(I2C1, SlaveAddress, GYRO_ZOUT_H);
             data[1]=data[1]<<8;
            uart_putchar (UART1, 0);
            data[0]=I2C_ReadAddr(I2C1, SlaveAddress, GYRO_ZOUT_L);
            y_data=data[1]|data[0];
        if(y_data>0x7fff)
                y_data=y_data-65535;
        else
                y_data=y_data-0;
            
           
          
             y_sum=y_sum+ y_data;
           }
           
           
            y_aver=((double)y_sum)/3.0;
            y_anglespeed=y_aver*FS;
            y_sum=0;
         
return(y_anglespeed);
}






  int x_data,z_data;
  long int x_sum,z_sum;
  double x_aver,x_anglespeed,z_aver,z_anglespeed;
  float xz_anglespeed_new,xz_anglespeed_old;
//  extern float delta;
double xz_anglespeed()
{
 double z_anglespeed=0; 
//I2C_WriteAddr(I2C1, SlaveAddress, PWR_MGMT_1, 0x00);    
for(int n=0;n<3;n++)
          {   data[1]=I2C_ReadAddr(I2C1, SlaveAddress, GYRO_ZOUT_H);
             data[1]=data[1]<<8;
             uart_putchar (UART1, 0);
             data[0]=I2C_ReadAddr(I2C1, SlaveAddress, GYRO_ZOUT_L);
               
            z_data=data[1]|data[0];
        if(z_data>0x7fff)
                z_data=z_data-65535;
        else
                z_data=z_data-0;
            
                   
             z_sum=z_sum+ z_data;
           }

          z_aver=((double)z_sum)/3.0;
          z_anglespeed=z_aver*FS;
          z_sum=0;
return( z_anglespeed);
            
            
            
            
            
            
            
            
            
            
            

}
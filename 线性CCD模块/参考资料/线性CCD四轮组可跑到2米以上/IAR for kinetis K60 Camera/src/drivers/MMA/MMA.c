#include "common.h"
#include  "include.h"
#include  "MMA.h"
#define   m    9

void MMA_init()
{
I2C_init(I2C0);
I2C_WriteAddr(I2C0, MMA7660_ADDRESS, MMA7660_MODE, 0x00);    
I2C_WriteAddr(I2C0, MMA7660_ADDRESS,MMA7660_SPCNT,240) ;    
I2C_WriteAddr(I2C0, MMA7660_ADDRESS,MMA7660_INTSU,0x02) ;
I2C_WriteAddr(I2C0, MMA7660_ADDRESS,MMA7660_PDET,0x00); 
I2C_WriteAddr(I2C0, MMA7660_ADDRESS,MMA7660_SR,0x0);
I2C_WriteAddr(I2C0, MMA7660_ADDRESS,MMA7660_PD,0x0);
I2C_WriteAddr(I2C0, MMA7660_ADDRESS,MMA7660_MODE,0x39);

}

 
  int Temp[m]={0},temp,value;
  char count=0;
  int i,j;
  int Z_data;
  double MMA7660_uint=0.046875; //MMA7660的量程1.5g/32得到的，就是每1的g大小
double angle,f_angle;
double z_angle()
{

  
   for ( count=0;count<m;count++)  
   {  	
	
          
         Z_data =I2C_ReadAddr(I2C0, MMA7660_ADDRESS, MMA7660_ZOUT);
         
         
         
         
         
         
         if( Z_data>31 )
             Z_data=Z_data-64;
          else
              Z_data=Z_data-0;
          
          Temp[count]=Z_data;
         //  uart_putchar (UART1, 0);
        
        
   }  
	 
  for (j=0;j<m;j++)  
   {  
      for (i=0;i<m-j;i++)  
      {  
         if ( Temp[i]>Temp[i+1] )  
         {  
            temp= Temp[i];  
            Temp[i]= Temp[i+1];   
             Temp[i+1] = temp;  
         }  
      }  
   }
   f_angle=Temp[(m-1)/2]*MMA7660_uint;
 if(f_angle>0.99)f_angle=0.99;
 if(f_angle<-0.99)f_angle=-0.99;
 angle=asin(f_angle)*180/3.14;
 
  return angle;
	 
	   //AD_Value[3]= value_buf3[(N-1)/2];  
}

   
/*
int Z_data;
int Z_sum;
double Z_aver,angle;
double MMA7660_uint=0.046875; //MMA7660的量程1.5g/32得到的，就是每1的g大小 
double z_angle()
{
      
        
         for(int n=0;n<40;n++)
    {
          Z_data=I2C_ReadAddr(I2C0, MMA7660_ADDRESS, MMA7660_ZOUT);
         if( Z_data>31 )
             Z_data=Z_data-64;
          else
              Z_data=Z_data-0;
          
          Z_sum=Z_sum+Z_data;   
       
        uart_putchar (UART1, 0);
          
          
    }
 
       
         Z_aver=((double)Z_sum)/40.0;
         
        angle=0-asin(Z_aver*MMA7660_uint)*180/3.14;

        Z_sum=0;
         Z_data=0;


         
        
return(angle);



}

*/





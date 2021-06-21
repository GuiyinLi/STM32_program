

#include "common.h"
#include "include.h"
#include "delay.h"
#include "ov7725.h"
#include "procsped.h"

void sobel(u8 (*img)[CAMERA_W],u8 (*imgout)[CAMERA_W],u16 tresh)
{

  u16 c,r,h,w;
  long int fx,fy,grad;
  h=CAMERA_H-1;
  w=CAMERA_W-1;
  for(c=1;c<h;c++)
  {
    for(r=1;r<w;r++ )
    {
     
     fx=*(*(img+c+1)+r-1)+*(*(img+c+1)+r)<<1+*(*(img+c+1)+r+1)-*(*(img+c-1)+r-1)-*(*(img+c-1)+r)<<1-*(*(img+c-1)+r+1);
     fy=*(*(img+c-1)+r+1)+*(*(img+c)+r+1)<<1+*(*(img+c+1)+r+1)-*(*(img+c-1)+r-1)-*(*(img+c)+r-1)<<1-*(*(img+c+1)+r-1);
     if(fx<0)
     {
       fx=-fx;
        
     }
     if(fy<0)
     {
        fy=-fy;
     }
     grad=fx+fy;
     
     if(grad>=tresh)
      *(*(imgout+c)+r)=255;
    else 
      *(*(imgout+c)+r)=0;
    }
    
    
  }
     

}
#include "common.h"
#include "include.h"
#include "imgproc.h"
#include "arm_math.h"
void edge_sobel(u16 **img,u8 tresh,u8 **imgout ,u8 wideth,u8 height)
{
  u16 c,r,h,w;
  long int fx,fy,grad;

  h=height-1;
  w=wideth-1;
  
  for(c=1;c<h;c++)
  {
    for(r=1;r<w;r++ )
    {
     
     fx=img[c+1][r-1]+img[c+1][r]<<1+img[c+1][r+1]-img[c-1][r-1]-img[c-1][r]<<1-img[c-1][r+1];
     fy=img[c-1][r+1]+img[c][r+1]<<1+img[c+1][r+1]-img[c-1][r-1]-img[c][r-1]<<1-img[c+1][r-1];
     if(fx<0)
     {
       fx=-fx;
        
     }
     if(fy<0)
     {
        fy=-fy;
     }
     grad=fx+fy;
    }
    if(grad>=tresh)
      imgout[c][r]=255;
    else 
      imgout[c][r]=0;
  }
}
/********************************** RP  ********************************************
 * File name       £∫main_uarttest.c
 * Description        £∫test file
**********************************************************************************/
#include "common.h"
#include "include.h"
#include "SCI.h"
#include <stdbool.h> //≤º∂˚‘ÀÀ„ø‚
u8 imageData[80][80];

void main(void)
{
  u8 i,j;
  sci_init(UART4);
  				
  while(1)
  {
    for(i=0 ; i<79 ; i++)
      for(j=0 ; j<79 ; j++)
      {
        imageData[i][j]=j*2;
      }
  
   sent_image(imageData);
    LED_test();
  }
}

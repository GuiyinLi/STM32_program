#ifndef __MMA_H__
#define __MMA_H__

#include "common.h"


#define   MMA7660_ADDRESS       (0x4C)    //MMA7660µÄdevice_address

/*MMA7660 Register Address*/
#define         MMA7660_XOUT    0x00
#define         MMA7660_YOUT    0x01
#define         MMA7660_ZOUT    0x02
#define         MMA7660_TILT    0x03
#define         MMA7660_SRST    0x04
#define         MMA7660_SPCNT   0x05
#define         MMA7660_INTSU   0x06
#define         MMA7660_MODE    0x07
#define         MMA7660_SR      0x08
#define         MMA7660_PDET    0x09
#define         MMA7660_PD      0x0A

void MMA_init(void);
double z_angle();





#endif  
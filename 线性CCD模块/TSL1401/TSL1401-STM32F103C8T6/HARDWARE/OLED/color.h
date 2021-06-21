#ifndef __color_H
#define __color_H		

#include "sys.h"
// RGB:	RED:0-32d	Green:0-64d	Blue:0-32d
#define RGB(R,G,B)  (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))
typedef  enum {
    Black     						= RGB(  0,  0,  0), 		// black
    Grey      						= RGB(192,192,192), 		// grey
    White     						= RGB(255,255,255), 		// white
    Red       						= RGB(255,  0,  0), 		// red
    Pink      						= RGB(255,192,203), 		// pink
    Yellow    						= RGB(255,255,  0), 		// yellow
    Golden    						= RGB(255,215,  0), 		// golden
    Brown     						= RGB(128, 42, 42), 		// brown
    Blue      						= RGB(  0,  0,255), 		// blue
    Cyan      						= RGB(  0,255,255), 		// cyan
    Green     						= RGB(  0,255,  0), 		// green
    Purple    						= RGB(160, 32,240), 		// purple
		RosyBrown							=	RGB(238,180,180), 		// RosyBrown
		MediumOrchid					=	RGB(186, 85,211), 		// MediumOrchid
}ColorType;
#endif

#ifndef _3DERRORS_H_
#define _3DERRORS_H_

//Usefull defines/macros
#define PI 3.141592653589793F
#define RADIAN(x) ((x)*PI/180.0f)
#define DEGREE(x) ((x)*180.0f/PI)

//Error codes

//flags
#define SCC_DOUBLEBUFFER        0x01 //Default
#define SCC_TRIPLEBUFFER        0x02 //ignored if SCC_BACKBUFFERINSYSMEM is specified
#define SCC_QUADRUPLEBUFFER     0x04 //Also ignored with SCC_BACKBUFFERINSYSMEM
#define SCC_FULLSCREEN          0x08 //Default
#define SCC_WINDOWED            0x10
#define SCC_BACKBUFFERINSYSMEM  0x20
#define SCC_NOVSYNC			    0x40


#endif
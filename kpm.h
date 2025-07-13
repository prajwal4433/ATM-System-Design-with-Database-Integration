// keypad matrix -header.h file 
#ifndef _KPM_H
#define _KPM_H

#include "types.h"

//macros
///row arr,col arr for non continous pins
#define ROWS 4 //->o/p
#define COLS 4 //->i/p


#define KPM_R0 24 //@P1.[24-31]
#define KPM_R1 25
#define KPM_R2 26
#define KPM_R3 27
#define KPM_C0 28 
#define KPM_C1 29
#define KPM_C2 30
#define KPM_C3 31

//kpm labels
extern s32 label2D[ROWS][COLS];
extern s32 label3D[3][COLS][COLS];

//functions
void initKpm4x4(void);
s32 readKpm4x4(void);
s32 readKpm4x4x4(void);



#endif


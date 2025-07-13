//software delays library
#include "types.h"

void delayS(u32 dly){
	dly*=12000000;
	while(dly--);
}

void delayMs(u32 dly){
	dly*=12000;
	while(dly--);
}

void delayUs(u32 dly){
	dly*=12;
	while(dly--);
}

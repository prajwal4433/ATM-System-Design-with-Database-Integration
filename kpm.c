// keypad matrix -definition.c file 
#include <lpc214x.h>
#include "types.h"
#include "gpio.h"
#include "kpm.h"
#include "delay.h"

//atm kepad
s32 label2D[4][4]=
{
	{'1'  ,'2','3' ,'A'},// * ->'\b'
	{'4'  ,'5','6' ,'B'},// # ->'\n'
	{'7'  ,'8','9' ,'C'},// A ->up	,C ->cancel
  	{'\b' ,'0','\n','D'} // B ->dn
};

//functions
void initKpm4x4(void){
	//rows as output pins
	INIT4PINS(GPIO1,KPM_R0,0xF);
//	INIT4PINS(GPIO1,KPM_C0,0x0);
}

s32 readKpm4x4(void ){
	s16 i,j,stat=0;
	for(i=0;i<ROWS;i++){
		//send 
		WRITE4PINS(GPIO1,KPM_R0,((1<<i)^0xF));
		//read cols
		stat=READ4PINS(GPIO1,KPM_C0);//default high
		if(stat!=0xF){
			//press found
			//wait till released
			while(READ4PINS(GPIO1,KPM_C0)!=0xF);
			//single press checks
			for(j=0;j<COLS;j++){
				if(!((stat>>j)&1)){
					delayMs(300);
					return label2D[i][j];
				}
			}
		}
	}
	return 0;
}

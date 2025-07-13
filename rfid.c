
#include <string.h>
#include "types.h"
#include "uart.h"

//assuming uart1 is already initiated

void getCardNo(s8 *buf){
	u32 i=0;
	s8 ch;
	do{
		ch=rx_UART(U1);
		if(ch)buf[i++]=ch;
	}while(ch!=0x03);
	buf[i]='\0';
}

s32 isCardNoOk(s8 *buf){
 	if(strlen(buf)!=10)return 0;
	if((buf[0]!=0x02)||(buf[9]!=0x03))return 0;
	return 1;
}


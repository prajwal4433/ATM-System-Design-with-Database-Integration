#include <lpc214x.h>
#include "types.h"
#include "uart.h"
#include "atm.h"

void initUART(void){
		PINSEL0=((PINSEL0&~(0xF<<U1TX*2))|(5<<U1TX*2));
		U1LCR = ((1<<UART_DLAB)|(UART_WRDLEN));
		U1DLL = DVSR;
		U1DLM = DVSR>>8;
		U1LCR&= ~(1<<UART_DLAB);
		PINSEL0=((PINSEL0&~0xF)|5);
		U0LCR = ((1<<UART_DLAB)|(UART_WRDLEN));
		U0DLL = DVSR;
		U0DLM = DVSR>>8;
		U0LCR&= ~(1<<UART_DLAB);
#ifdef UART_INT
	VICIntSelect = 0; 		// IRQ
	VICVectAddr0 = (unsigned)UART0_isr;
	VICVectCntl0 = 0x20 | 6;  	//UART0 Interrupt
	VICIntEnable = 1 << 6;   	// Enable UART0 Interrupt 
	U0IER = 0x01;       		// Enable UART0 RDA Interrupts 
#endif
}
//
void txUART(u8 un,u8 word){
	if(un){
		U1THR=word;
		while(((U1LSR>>UART_TEMT)&1)==0);
	}else{
		U0THR=word;
		while(((U0LSR>>UART_TEMT)&1)==0);
	}
}
//


u8 rx_UART(u8 un){
	static u8 word;
	word=0;
	if(un){
		if((U1LSR>>UART_DR)&1)
			word=U1RBR;
	}else{
		if((U0LSR>>UART_DR)&1)
			word=U0RBR;
	}
	return word;
}
//

void strRx_UART(u8 un,s8* buf){
	u32 i=0;
	s8 ch;
	do{
		ch=rx_UART(U0);
		if(ch)buf[i++]=ch;
	}while(ch!='\n');
	buf[i-1]='\0';
}


//
void  strTxUART(u8 un,s8 *str){
	while(*str)
		txUART(un,*(str++));
}
//



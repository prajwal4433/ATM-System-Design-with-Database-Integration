

#ifndef _UART_H_
#define _UART_H_

#include "types.h"

#define FOSC 12000000
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)
#define BAUD 9600
#define DVSR (PCLK/(16*BAUD))

#define U0 0
#define U1 1

#define U0TX 0
#define U0RX 1
#define U1TX 8
#define U1RX 9

//SFR's bit nums
///UxLCR
#define UART_DLAB	7
#define UART_WRDLEN 3//8 BIT
///UxLSR
#define UART_TEMT	6
#define UART_DR		0

void initUART(void);
void txUART(u8,u8);
u8	 rxUART(u8);
u8 	 rx_UART(u8 un);

void charTxUART(u8,s8 );
void  strTxUART(u8,s8*);

s8  charRxUART(u8);
void strRxUART(u8,s8*);
//
void strRx_UART(u8 un,s8* buf);

#endif


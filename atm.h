#ifndef _ATM_H
#define _ATM_H

#include <lpc214x.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "kpm.h"
#include "lcd.h"
#include "uart.h"
#include "delay.h"
#include "rfid.h"
#include "atm.h"

#define UART_INT
#define CC (const char*)

//atm limits
#define MAX_TRYS 		3
#define MAX_DEPOSIT     30000 //30K
#define MAX_WITHDRAW    30000 //30k
#define MAX_TRANSFER    100000//1lk

//str_KPM
#define BUF_MAX 50
#define PIN 1
#define STR 0

#define DISP_TIME 1000 	//1sec
#define ATM_TIME  (30*120000) //30 SEC

extern volatile s8 buf[],dummy;
extern volatile u32 r_flag,time,buf_index;

void sys_init(void);

void UART0_isr(void) __irq;

void checkPC(void);
void sendMsg(s8 *str);
void getMsg( s8 *str);
s32  isMsgOk(s8 *str);
u32  getPin( s8 *str);
int str_KPM( s8 *str,u8 row);

void atm_wtd(s8 *rfid,s8 *buf);
void atm_dep(s8 *rfid,s8 *buf);
void atm_bal(s8 *rfid,s8 *buf);
void atm_mst(s8 *rfid,s8 *buf);
int  atm_pin(s8 *rfid,s8 *pin,s8 *buf);

#endif




#ifndef _RTC_H_
#define _RTC_H_

#include "types.h"

#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define FRTC 32768

#define INT ((s32)(PCLK/FRTC -1))
#define FRAC (PCLK-((INT+1)*FRTC))

#define F_12HRS 1
#define F_24HRS 0

//CCR
#define CLKEN  0
#define CTCRST 1
#define CLKSRC 4

//ILR
#define RTCCIF 0
#define RTCALF 1
//CIIR
#define IMSEC	0
#define IMMIN	1
#define IMHOUR	2
#define IMDOM	3
#define IMDOW	4
#define IMDOY	5
#define IMMON	6
#define IMYEAR	7
//AMR
#define AMRSEC	0
#define AMRMIN	1
#define AMRHOUR	2
#define AMRDOM	3
#define AMRDOW	4
#define AMRDOY	5
#define AMRMON	6
#define AMRYEAR	7


//
extern s8 day[][4];

void init_RTC(void);
void set_RTC(u16 y,u8 m,u8 d,u8 h,u8 min,u8 s);
void getTime(s8*,u8);//HH:MM:SS AM/PM
void getDate(s8*);	 //DD/MM/YYYY
//void getAll_RTC(s8*);//HH:MM:SS AM/PM, ___DAY, month, DD/MM/YYY,doy

void setAlarm(u8 h,u8 m);

#endif


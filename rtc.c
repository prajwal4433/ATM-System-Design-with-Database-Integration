#include <lpc214x.h>
#include "types.h"
#include "rtc.h"

s8 day[][4]={"MON","TUE","WED","THR","FRI","SAT","SUN"};

void init_RTC(void){
	CCR = (1<<CTCRST);
	PREINT =INT;
	PREFRAC=FRAC;
	CCR = (1<<CLKEN);
}
//
void set_RTC(u16 y,u8 m,u8 d,u8 h,u8 min,u8 s){
	//date
	YEAR=y;
	MONTH=m;
	DOM=d;
	DOW=0,DOY=0;///calc
	
	//clock
	HOUR=h;
	MIN=min;
	SEC=s;
}
void getTime(s8 *str,u8 format){
	//HH:MM:SS AM/PM -11 chars
	if(format){
		if(HOUR>12){
			str[0]='0'+((HOUR-12)/10);
			str[1]='0'+((HOUR-12)%10);
		}else if(!HOUR){
			str[0]='1',str[1]='2';
		}
	}else{
		str[0]='0'+(HOUR/10);
		str[1]='0'+(HOUR%10);
	}
	str[2]=':';
	str[3]='0'+(MIN/10);
	str[4]='0'+(MIN%10);
	str[5]=':';
	str[6]='0'+(SEC/10);
	str[7]='0'+(SEC%10);
	if(format){
		str[8]=' ';
		str[9]=(HOUR<12)?'A':'P';
		str[10]='M';
		str[11]='\0';
	}else{
		str[8]='\0';
	}
}

void getDate(s8 *str){
	//DD/MM/YYYY -10 chars
	str[0] ='0'+(DOM/10);
	str[1] ='0'+(DOM%10);
	str[2] ='/';
	str[3] ='0'+(MONTH/10);
	str[4] ='0'+(MONTH%10);
	str[5] ='/';
	str[6] ='0'+(YEAR/1000)%10;
	str[7] ='0'+(YEAR/100)%10;
	str[8] ='0'+(YEAR/10)%10;
	str[9] ='0'+(YEAR/1)%10;
	str[10]='\0';
}

//
void setAlarm(u8 h,u8 m){
	ALMIN=m;
	ALHOUR=h;
	ALSEC=0;
	AMR=~((1<<AMRHOUR)|(1<<AMRMIN)|(1<<AMRSEC));
	ILR= 1<<RTCALF;
}
//
void getAll_RTC(s8*);//HH:MM:SS AM/PM, ___DAY, month, DD/MM/YYY,doy

void int2str(s8 *str,u32 n){
	do{
		*(str++)=(n%10)+'0';
		n/=10;
	}while(n);
}

#include <string.h>
#include "atm.h"

#define ISNUM(ch) ((ch>='0')&&(ch<='9'))

volatile s8 buf[BUF_MAX]="",dummy;
volatile u32 r_flag,time,buf_index;

void sys_init(void){
	//initiate peripherals
	initUART();		//uart0,1
	initLcd();	 	//lcd
	initKpm4x4();	//kpm
					//buzzer
}

void UART0_isr(void) __irq {
    if ((U0IIR & 0x0E) == 0x04) { // RDA interrupt
        char ch = U0RBR;
        if (!r_flag){
            if(ch=='\n'){
		    buf[buf_index-1] = '\0';// '\r'
		    if(!strcmp((const char*)buf,"@Y:LINEOK$")){
			    sendMsg("#Y:LINEOK$");
		    }else{
			    r_flag=1;
		    }
		    buf_index=0;
	    } else if(buf_index < BUF_MAX-1) {
                buf[buf_index++]=ch;
            }
        }
    }

    dummy = U0IIR;
    VICVectAddr = 0;
}
//
void sendMsg(s8 *str){
	strTxUART(U0,str);
	strTxUART(U0,"\r\n");
#ifdef DBG
	moveLcdCursor(1,0);	
	str2Lcd("Tx:");
	str2Lcd(str);
	str2Lcd(".");
	delayS(1);
#endif
}

void getMsg(s8 *str){
	//includes '\r'
	strRx_UART(U0,str);
	//removes '\r'
	str[strlen(str)-1]='\0';
#ifdef DBG
	moveLcdCursor(1,0);	
	str2Lcd("Rx:");
	str2Lcd(str);
	str2Lcd(".");
	delayS(1);
#endif
}

s32 isMsgOk(s8 *str){
	if((str[0]!='@')||(str[strlen(str)-1]!='$'))return 0;
	return 1;
}

u32 getPin(s8 *str){
	s32 i=0;
	u32 time=ATM_TIME;
	s8 ch;
	clearLcdRow(1);
	while(i<4 && time){
		while(!(ch=readKpm4x4())){
			if(time)--time;
			else return 0;
		}
		if(ISNUM(ch)){
			char2Lcd(ch);
			delayMs(300);
			moveLcdCursor(1,i);
			char2Lcd('*');
			str[i++]=ch;
			time=ATM_TIME;
		}else if(ch=='\b'){
			if(i){
				--i;
				moveLcdCursor(1,i);
				char2Lcd(' ');
				moveLcdCursor(1,i);
			}
			time=ATM_TIME;
		}else if(ch=='C'){
			return -1;
		}
	}
	str[i]='\0';
	
	return time?1:0;
}

int str_KPM(s8 *str,u8 row){
	u32 i=0;
	s8 ch='\0';
	moveLcdCursor(row,0);
	while(i<BUF_MAX-1 && time){
		ch=readKpm4x4();
		if(ch)time=ATM_TIME;
		else{
			time--;
			continue;
		}
		if(ch=='\b'){
			str[i]='\0';
			moveLcdCursor(row,i);
			char2Lcd(' ');
			moveLcdCursor(row,i);
			if(i)--i;
		}else if(i && ch=='\n'){
			str[i]='\0';
			return 0;//valid str+enter
		}else if(ISNUM(ch)){
			if(!i && (ch=='0'))continue;
			str[i]=ch;
			char2Lcd(ch);
			i++;
		}else if(ch=='C'){
		 	return -1;
		}
	}
	//session time-out
	return 1;
}

void atm_wtd(s8 *rfid,s8 *buf){
	s8 amt[20];
	clearLcdDisplay();
	str2Lcd("Withdraw amt:");
	//take amt input
	if(str_KPM(amt,1)==-1){
		//cancel
		moveLcdCursor(0,0);
		str2Lcd(" Cancelled !!!! ");
		clearLcdRow(1);
		delayS(2);
		return;		
	}
	if((amt[strlen(amt)-2]!='0')||(amt[strlen(amt)-1]!='0')){
		//invlaid ammt
		moveLcdCursor(0,0);
		str2Lcd(" Amount Must be ");
		moveLcdCursor(1,0);
		str2Lcd("100Rs multiples!");
		delayS(2);
		return;
	}
	//try withdraw
	checkPC();
	//#A:WTD:<rfid>:<amt>$
	sprintf(buf,"#%c:WTD:%s:%s$",'A',rfid,amt);
	sendMsg(buf);
#ifdef UART_INT
	while(!r_flag);
	r_flag=0;
#else
	getMsg(buf);
#endif
	//intrepret msg
	//@OK:DONE$,@ERR:LOWBAL$,@ERR:NEGAMT$,@ERR:MAXAMT$
	if(!strcmp(buf,"@OK:DONE$")){
		//ok
		moveLcdCursor(0,0);
		str2Lcd("Amount Withdrawn");
		moveLcdCursor(1,0);
		str2Lcd("  Succesfully!! ");
		delayS(2);
	}else if(!strcmp(buf,"@ERR:LOWBAL$")){
		//Low balance
		moveLcdCursor(0,0);
		str2Lcd("Withdraw Failed ");
		moveLcdCursor(1,0);
		str2Lcd("  Low-Balance!  ");
	}else if(!strcmp(buf,"@ERR:NEGAMT$")){
		//negative wtd amt

	}else if(!strcmp(buf,"@ERR:MAXAMT$")){
		//amt exceed max limit
		moveLcdCursor(0,0);
		str2Lcd("Withdraw Failed ");
		moveLcdCursor(1,0);
		str2Lcd("Exceeds MaxLimit");
	}else{
		//wrng content
	}
	delayS(2);
}

//

void atm_dep(s8 *rfid,s8 *buf){
	s8 amt[20];
	clearLcdDisplay();
	str2Lcd("Deposit amt:");
	//take amt input
	if(str_KPM(amt,1)==-1){
		//cancel
		moveLcdCursor(0,0);
		str2Lcd(" Cancelled !!!! ");
		clearLcdRow(1);
		delayS(2);
		return;		
	}
	//try deposit
	checkPC();
	//#A:DEP:<rfid>:<amt>$
	sprintf(buf,"#%c:DEP:%s:%s$",'A',rfid,amt);
	sendMsg(buf);
#ifdef UART_INT
	while(!r_flag);
	r_flag=0;
#else	
	getMsg(buf);
#endif
	//intrepret msg
	//@OK:DONE$,@ERR:NEGAMT$,@ERR:MAXAMT$
	if(!strcmp(buf,"@OK:DONE$")){
		//ok
		moveLcdCursor(0,0);
		str2Lcd("Amount Deposited");
		moveLcdCursor(1,0);
		str2Lcd("  Succesfully!! ");
		
	}else if(!strcmp(buf,"@ERR:NEGAMT$")){
		//negative dep amt
	}else if(!strcmp(buf,"@ERR:MAXAMT$")){
		//amt exceed max limit
		moveLcdCursor(0,0);
		str2Lcd("Withdraw Failed ");
		moveLcdCursor(1,0);
		str2Lcd("Exceeds MaxLimit");
		
	}else{
		//wrng content
	}
	delayS(2);
}

void atm_bal(s8 *rfid,s8 *buf){
	s32 i=0;
	clearLcdDisplay();
	str2Lcd("Balance:");
	//get balance
	checkPC();
	while(1){
		//#A:BAL:<rfid>$
		sprintf(buf,"#%c:BAL:%s$",'A',rfid);
		sendMsg(buf);
#ifdef UART_INT
		while(!r_flag);
		r_flag=0;
#else
		getMsg(buf);
#endif
		//rcvd msg format check
		if(isMsgOk(buf))break;
	}
	
	//intrepret msg
	//@OK:BAL=<amt>$
	if(!strncmp(buf,"@OK:BAL=",7)){
		//ok and extract balance
		i=0;
		while(1){
			if(buf[i+8]=='$'){
				buf[i]='\0';
				break;
			}
			buf[i]=buf[i+8];
			i++;
		}
		//display
		clearLcdRow(1);
		str2Lcd(buf);
		str2Lcd(" Rs");
		delayS(2);
	}else{
		//wrng content
	}
}
//

int  atm_pin( s8 *rfid,s8 *pin,s8 *buf){
	s8 dum[5]={0};
	s32 ret;
	clearLcdDisplay();
	str2Lcd("Enter Old pin:");
	ret=getPin(buf);
	if(!ret){
		//timed-out
		moveLcdCursor(0,0);
		str2Lcd("Session Time-Out");
		moveLcdCursor(1,0);
		str2Lcd("   Thank you.   ");
		delayS(2);
		return 0;
	}else if(ret==-1){
		//cancelled 
		moveLcdCursor(0,0);
		str2Lcd(" Cancelled !!!! ");
		clearLcdRow(1);
		delayS(2);
		return 0;		
	}
	//check
	if(strcmp(pin,buf)){
		//failed
		clearLcdDisplay();
		str2Lcd("Incorrect pin!!");
		return 0;
	}
	//new pin
	clearLcdDisplay();
	str2Lcd("Enter New pin:");
	ret=getPin(buf);
	if(!ret){
		//timed-out
		moveLcdCursor(0,0);
		str2Lcd("Session Time-Out");
		moveLcdCursor(1,0);
		str2Lcd("   Thank you.   ");
		delayS(2);
		return 0;
	}else if(ret==-1){
		//cancelled 
		moveLcdCursor(0,0);
		str2Lcd(" Cancelled !!!! ");
		clearLcdRow(1);
		delayS(2);
		return 0;		
	}
	//re-enter new pin
	clearLcdDisplay();
	str2Lcd("Re-enter New pin");
	ret=getPin(dum);
	if(!ret){
		//timed-out
		moveLcdCursor(0,0);
		str2Lcd("Session Time-Out");
		moveLcdCursor(1,0);
		str2Lcd("   Thank you.   ");
		delayS(2);
		return 0;
	}else if(ret==-1){
		//cancelled 
		moveLcdCursor(0,0);
		str2Lcd(" Cancelled !!!! ");
		clearLcdRow(1);
		delayS(2);
		return 0;		
	}
	//verify
	if(strcmp(buf,dum)){
		//mismatched
		clearLcdDisplay();
		str2Lcd("New pin mismatch");
		delayS(1);
		return 0;
	}
	strcpy(pin,dum);
	//send pin change
	checkPC();
	while(1){
		//#A:PIN:<rfid>:<pin>$
		sprintf(buf,"#%c:PIN:%s:%s$",'A',rfid,pin);
		sendMsg(buf);
#ifdef UART_INT
		while(!r_flag);
		r_flag=0;
#else
		getMsg(buf);
#endif
		//rcvd msg format check
		if(isMsgOk(buf))break;
	}
	
	//intrepret msg
	//@OK:DONE$
	if(!strcmp(buf,"@OK:DONE$")){
		//ok
		return 1;
	}else{
		//wrng content
		clearLcdDisplay();
		str2Lcd("Unknown Error");
		return 0;
	}
}
//
void atm_mst(s8 *rfid,s8 *buf){
	s32 i=0,j=0;
	s8 t[][4]={"WTD","DEP","TIN","TOT"};
	
	//get txn by txn
	checkPC();
	while(j<3){
		//#A:MST:<rfid>:<txNo>$
		sprintf(buf,"#%c:MST:%s:%d$",'A',rfid,j+1);
		sendMsg(buf);
		//@:TXN:<type>:<amt>$
#ifdef UART_INT
		while(!r_flag);
		r_flag=0;
#else
		getMsg(buf);
#endif
		//rcvd msg format check
		if(isMsgOk(buf)){
		 	//extrct tx amt,type
			if(buf[5]=='7'){
			 	//no more txns
			 	break;
			}else{
				//"@TXN:%d:%02u/%02u/%04u %02u:%02u:%.2lf$",t->type,dd,mon,yy,hh,mm,amt
				moveLcdCursor(0,0);
				for(i=0;i<16;i++){
					char2Lcd(buf[i+7]);
				}
				clearLcdRow(1);
				moveLcdCursor(1,0);
				for(i=0;buf[i+24]!='$';i++){
					char2Lcd(buf[i+24]);
				}
				moveLcdCursor(1,13);
				str2Lcd(t[buf[5]-'1']);
				delayS(2);
			}
			 ++j;
		}
	}
}
//

void checkPC(void){
	while(1){
		sendMsg("#X:LINEOK$");
#ifdef UART_INT
		while(!r_flag);
		r_flag=0;
#else
		getMsg(buf);
#endif
			/*
			clearLcdDisplay();
			moveLcdCursor(0,0);
			str2Lcd("~");
			str2Lcd((const char*)buf);
			char2Lcd('~');
			char2Lcd(strlen((const char*)buf)+'a');
			delayS(2);
			*/
			

		if(!strcmp((const char*)buf,"@X:LINEOK$")){
		/*
			moveLcdCursor(1,0);
			str2Lcd("Pc line ok bro .");
		*/
			break;
		
		}else{
			moveLcdCursor(1,0);
			str2Lcd("waiting for PC.");
		}
		delayS(2);
	}
}

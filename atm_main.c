#include "atm.h"

const s8 menu[6][16]={"1.WITHDRAW CASH","2.DEPOSIT CASH","3.VEIW BALANCE","4.MINI STATMENT","5.PIN CHANGE","6.EXIT ATM"};

s32 main(){

	s8 rfid[9]="11111111",pin[5]="1111",ch;
	s32 trys,curRow=0,prevRow=-1,errNo;
	sys_init();
	//welcome screen
	moveLcdCursor(0,0);
	str2Lcd(" Welcome To ATM ");
	sendMsg(" Welcome To ATM ");
	delayS(1);

	//checkPC();
	//goto A;//testing :)
	//super loop
	while(1){
		moveLcdCursor(0,0);
		str2Lcd("Place the card  ");
		moveLcdCursor(1,0);
		str2Lcd("on the reader.. ");
		//wait for rfid
		getCardNo(buf);
		
		if(!isCardNoOk(buf)){
		 //place again
		 delayS(1);
		 continue;
		}
		
		//extract rfid
		strncpy(rfid,(const char*)buf+1,8);
		clearLcdDisplay();
		str2Lcd("RFID:");
		str2Lcd(rfid);
		//check in DB
		checkPC();
		
		while(1){
			//#C:<rfid>$
			sprintf(buf,"#%c:%s$",'C',rfid);
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
		//@OK:ACTIVE:<name>$,@ERR:INVALID$,@ERR:BLOCK$
		if(!strncmp((const char*)buf,"@OK:ACTIVE:",11)){
			//ok
			moveLcdCursor(0,0);
			str2Lcd("Welcome customer");
			moveLcdCursor(1,0);
			for(time=0;(buf[time+11]!='$')&&(time<16);time++){
				char2Lcd(buf[time+11]);
			}
			for(;time<16;time++)char2Lcd(' ');
			delayS(2);
		}else if(!strcmp((const char*)buf,"@ERR:BLOCK$")){
			//err card blocked
			moveLcdCursor(0,0);
			str2Lcd("Card is Blocked!");
			moveLcdCursor(1,0);
			str2Lcd("   Visit Bank.  ");
			delayS(2);
			continue;
		}else if(!strcmp((const char*)buf,"@ERR:INVALID$")){
			//err not present
			moveLcdCursor(0,0);
			str2Lcd(" Card not Found!");
			moveLcdCursor(1,0);
			str2Lcd("Register at bank");
			delayS(2);
			continue;
		}else{
			//wrng content
			continue;
		}
			
		//take pin and verify
		trys=MAX_TRYS;
		while(trys){
			if(trys!=MAX_TRYS){
				moveLcdCursor(0,0);
				str2Lcd("  Wrong Pin!!!  ");
				moveLcdCursor(1,0);
				char2Lcd('0'+trys);
				str2Lcd(" - Tries Left  ");
				delayS(2);
			}
			moveLcdCursor(0,0);
			str2Lcd("Enter pin:      ");
			clearLcdRow(1);
			//take pin from kpm
			errNo=getPin(pin);
			if(!errNo){
				//timed-out
				moveLcdCursor(0,0);
				str2Lcd("Session Time-Out");
				moveLcdCursor(1,0);
				str2Lcd("   Thank you.   ");
				delayS(2);
				break;
			}else if(errNo==-1){
				//cancel
				moveLcdCursor(0,0);
                str2Lcd("Session Canceled");
                moveLcdCursor(1,0);
                str2Lcd("   Thank you.   ");
                delayS(2);
                break;
			}
			//verify in DB
			checkPC();
			while(1){
				sprintf(buf,"#%c:%s:%s$",'V',rfid,pin);
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
			//@OK:MATCHED$,@ERR:WRONG$
		 	if(!strcmp((const char*)buf,"@OK:MATCHED$")){
		 		//ok
				break;
		 	}else if(!strcmp((const char*)buf,"@ERR:WRONG$")){
				//err wrng pin
				trys--;
		 	}else{
				//wrng content
				continue;
			}
		}
		if(errNo!=1)continue;

		
		if(!trys){
			//card blocked
			while(1){
				sprintf(buf,"#%c:BLK:%s$",'A',rfid);
				sendMsg(buf);
#ifdef UART_INT
				while(!r_flag);
				r_flag=0;
#else
				getMsg(buf);
#endif		
				//rcvd msg format check
				if(isMsgOk(buf)){
					if(!strcmp((const char*)buf,"@OK:DONE$")) break;
				}
			}
			clearLcdDisplay();
			str2Lcd(" Card Blocked!! ");
			moveLcdCursor(1,0);
			str2Lcd("   Visit Bank.  ");
			delayS(2);
			continue;
		}
A:
		//atm start
		prevRow=-1;
		curRow=0;
		trys=MAX_TRYS;
		time=ATM_TIME;
		while(time){
			//atm menu scrll
			if(curRow!=prevRow){
				clearLcdDisplay();
				str2Lcd(menu[curRow]);
				moveLcdCursor(1,0);
				str2Lcd(menu[curRow+1]);
				prevRow=curRow;
			}
			while(!(ch=readKpm4x4())){
				if(time)--time;
				else break;
			}
			
			if(ch){
				if(ch=='B'){
					//scroll dn
					if(curRow<6-2)curRow++;
				}else if(ch=='A'){
					//scroll up
					if(curRow)curRow--;
				}else if((ch==('0'+curRow+1))||(ch==('0'+curRow+2))){
					//ACTIONS
					if(ch=='1'){
						//withdraw
						atm_wtd(rfid,buf);
						prevRow=-1;
					}else if(ch=='2'){
						//deposit
						atm_dep(rfid,buf);
						prevRow=-1;
					}else if(ch=='3'){
						//balance
						atm_bal(rfid,buf);
						delayS(3);
						prevRow=-1;
					}else if(ch=='4'){
						//mini statement
						atm_mst(rfid,buf);
						prevRow=-1;
					}else if(ch=='5'){
						//pin change
						if(trys){
							//try pin change
							if(atm_pin(rfid,pin,buf)){
								//sucess
								clearLcdDisplay();
								str2Lcd("Pin changed.");
								trys=MAX_TRYS;
							}else{
								//failed
								moveLcdCursor(1,0);
								str2Lcd("Trys left: ");
								--trys;
								char2Lcd(trys+'0');
							}
						}
						delayMs(DISP_TIME);
						if(!trys){
							//contact bank branch
							while(1){
								sprintf(buf,"#%c:BLK:%s$",'A',rfid);
								sendMsg(buf);
#ifdef UART_INT
								while(!r_flag);
								r_flag=0;
#else
								getMsg(buf);
#endif		
								//rcvd msg format check
								if(isMsgOk(buf)){
									if(!strcmp((const char*)buf,"@OK:DONE$")) break;
								}
							}
							moveLcdCursor(0,0);
							str2Lcd(" Card Blocked!! ");
							moveLcdCursor(1,0);
							str2Lcd("   Visit Bank.  ");
							delayS(2);
							break;
						}
						prevRow=-1;
					}else if(ch=='6'){
						//exit
						checkPC();
						sendMsg("#Q:SAVE$");
						prevRow=-1;
						break;
					}
				}
				//
				time=ATM_TIME;
			}
			//
			if(time)--time;
		}
		if(time){
			//usr exited
			moveLcdCursor(0,0);
            str2Lcd("  Thank You !!  ");
            moveLcdCursor(1,0);
            str2Lcd("Have a nice Day.");
		}else{
				//atm closed due to inactivity
			moveLcdCursor(0,0);
			str2Lcd("Session Time-Out");
			moveLcdCursor(1,0);
			str2Lcd("   Thank you.   ");
		}
		delayS(2);
	}

}

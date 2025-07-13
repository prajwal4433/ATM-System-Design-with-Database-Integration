#include "types.h"
#include "delay.h"
#include "gpio.h"
#include "lcd.h"

//custom pixies
//const s8 bell[]		={0x04,0x0E,0x0E,0x0E,0x1F,0x00,0x04,0x00};

/////GENERAL 
void initLcd(){
	//GPIO cnfg
	//lcd RS pins as gpio_out
	INITPIN(GPIO0,LCD_RS,1);
	//lcd RW pins as gpio_out
	//INITPIN(GPIO1,LCD_RW,1);
	//lcd EN pins as gpio_out
	INITPIN(GPIO0,LCD_EN,1);
#if LCD_MODE==4
	INIT4PINS(GPIO1,LCD_PINS_4B,0xF);
#elif LCD_MODE==8
	INIT8PINS(GPIO1,LCD_PINS_8B,0xFF);
#endif
	////start lcd init
	delayMs(15);
	//8Bit-function set
	writeLcd(LCD_INIT8,LCD_CMD);
	delayMs(5);
	//8Bit-function set
	writeLcd(LCD_INIT8,LCD_CMD);
	delayUs(200);
	//8Bit-function set
	writeLcd(LCD_INIT8,LCD_CMD);
	delayUs(200);
#if LCD_MODE==4
	//4Bit-function set
	writeLcd(LCD_INIT4,LCD_INT);
#endif
	//set interface len
	writeLcd(LCD_DIM,LCD_CMD);
	//Display,Cursor,Block ON/OFF
	writeLcd(LCD_DISPON,LCD_CMD);
	//clear Display
	writeLcd(LCD_CLR,LCD_CMD);
	//Entry Mode
	writeLcd(LCD_SETCURINC,LCD_CMD);
	delayMs(2);
}
//
void writeLcd(s8 word,s16 dc){
	//data or cmd
	if(dc==1)SETPIN(GPIO0,LCD_RS);//dc=1->DATA
	else if(dc==0) CLRPIN(GPIO0,LCD_RS);//dc=0->CMD
	else if(dc==2){//dc=2->INIT
		CLRPIN(GPIO0,LCD_RS);
		//write upper nibble
		WRITE4PINS(GPIO1,LCD_PINS_4B,(word >> 4));
		//pulse to notify LCD
		SETPIN(GPIO0,LCD_EN);
		delayUs(1);
		CLRPIN(GPIO0,LCD_EN);
		return;
	}
#if LCD_MODE==8
	//WRITE BYTE
	WRITE8PINS(GPIO1,LCD_PINS_8B,word);
	//pulse to notify LCD
	SETPIN(GPIO0,LCD_EN);
	delayUs(1);
	CLRPIN(GPIO0,LCD_EN);
#elif LCD_MODE==4
	//write upper nibble
	WRITE4PINS(GPIO1,LCD_PINS_4B,(word >> 4));
	//pulse to notify LCD
	SETPIN(GPIO0,LCD_EN);
	delayUs(1);
	CLRPIN(GPIO0,LCD_EN);
	//write lower nibble
	WRITE4PINS(GPIO1,LCD_PINS_4B,(word & 0x0F));
	//pulse to notify LCD
	SETPIN(GPIO0,LCD_EN);
	delayUs(1);
	CLRPIN(GPIO0,LCD_EN);
#endif
	//wait to process
	//<=30 250khz fail
	delayUs(60);
}
//
void makeNewCharAt(const s8 *pixels,s16 asc){
	u16 i;
	if(asc>8)return;//invalid ascii
	//generate custom char at asc
	writeLcd(LCD_CGRAM+asc*8,LCD_CMD);
	for(i=0;i<8;i++)char2Lcd(pixels[i]);
}
//
void char2Lcd(s8 ch){
	writeLcd(ch,LCD_DATA);
}
//
void str2Lcd(const s8 *str){
	while(*str){
		writeLcd(*(str++),LCD_DATA);	
	}
}
//
void moveLcdCursor(s16 row,s16 col){
	//col [0-15]
	if(row==0)writeLcd(LCD_ROW0+col,LCD_CMD);		//go to row1, addr 0x00  ORed with 0x80: D7 must be set to move cursor
	else if(row==1)writeLcd(LCD_ROW1+col,LCD_CMD);	//go to row2
}
//

void clearLcdDisplay(){
    writeLcd(0x01,LCD_CMD);  // Clear display command
    delayMs(2);
}
//
void clearLcdRow(s16 row){
	u8 col;
	//set cursor
	if(row==0)writeLcd(LCD_ROW0,LCD_CMD);		//go to row1, addr 0x00  ORed with 0x80: D7 must be set to move cursor
	else if(row==1)writeLcd(LCD_ROW1,LCD_CMD);	//go to row2
	//clear row
	for(col=0;col<LCD_MAX_COLS;col++){  //columns per row
		writeLcd(' ',LCD_DATA);
    }
	//reset cursor
	if(row==0)writeLcd(LCD_ROW0,LCD_CMD);		//go to row1, addr 0x00  ORed with 0x80: D7 must be set to move cursor
	else if(row==1)writeLcd(LCD_ROW1,LCD_CMD);	//go to row2
}
//
void scrllStr2Lcd(s8*str,s16 row){
	s32 len=0,i=0,j,k,l,m;
	s8 *temp=str;
	//strlen implementation
	while(*temp){
		len++;
		temp++;
	}
	//scrlling
	while(1){
		j= LCD_MAX_COLS-1-((i++)%LCD_MAX_COLS);
		//strprint
		if(j){
			//right bound and Normal
			m=1;
		}else if(j==0){
			//left bound
			m=len;
		}
		for(l=0;l<m;l++){
				moveLcdCursor(row,j);
				for(k=0;k<LCD_MAX_COLS-j;k++){
					if(k+l>=len)break;
					char2Lcd(str[k+l]);
				}
				delayMs(60);
				clearLcdRow(row);
			}	
		
	}
}
//

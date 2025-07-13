#ifndef _LCD_H
#define _LCD_H

#include "types.h"

#define LCD_PINS_4B 20 	//@p1.[23-20]:D[7-4]
#define LCD_PINS_8B 16 	//@p1.[23-16]:D[7-0]

#define LCD_RS 	16		//@p0.16
#define LCD_RW 	26 		//@p1.26
#define LCD_EN 	17		//@p0.17

#define LCD_DATA 1
#define LCD_CMD  0
#define LCD_INT 2

//4-Bit mode
#define LCD 32		//16*2
#define LCD_MODE 8  //8 for 8-bit mode

//*****command hex macros*****//
//funcn set
#define LCD_INIT8 0x30
#define LCD_INIT4 0x20
//basic
#define LCD_CLR 0x01
#define LCD_HOMECUR 0x02
//SET CURSOR move direction
#define LCD_SETCURINC 0x06
#define LCD_SETDISPSHIFT 0x05
//cursor/display/blynk
#define LCD_DISPON 0x0C
#define LCD_DISPON_BLKON 0x0D
#define LCD_DISPON_CURON 0x0E
#define LCD_DISPON_CURON_BLKON 0x0F
//move cursor/shift display
//-shift cur/disp by 1 char l/r
#define LCD_DISPSHIFT_R 0x1C
#define LCD_DISPSHIFT_L 0x18
#define LCD_CURSHIFT_R 	0x14
#define LCD_CURSHIFT_L 	0x10
//interface
#if LCD_MODE==8
#define LCD_16x1 0x30
#define LCD_16x2 0x3C//0x3C,0x38 - big,small
#define LCD_20x4 0x38
#elif LCD_MODE==4
#define LCD_16x1 0x20
#define LCD_16x2 0x28
#define LCD_20x4 0x28
#endif

#if LCD==16
	#define LCD_DIM LCD_16x1
	#define LCD_MAX_ROWS 1
	#define LCD_MAX_COLS 16
#elif LCD==32
	#define LCD_DIM LCD_16x2
	#define LCD_MAX_ROWS 2
	#define LCD_MAX_COLS 16
#elif LCD==80
	#define LCD_DIM LCD_20x4
	#define LCD_MAX_ROWS 4
	#define LCD_MAX_COLS 20
#endif



//cursor to CGRAM
#define LCD_CGRAM 0x40
//cursor to DDRAM/display
#define LCD_ROW0 0x80//0x80|0x00
#define LCD_ROW1 0xC0//0x80|0x40
#define LCD_ROW2 0x94//0x80|0x14
#define LCD_ROW3 0xD4//0x80|0x54
//***

//custom chars
//extern const s8 bell[];

//*****FUNCTION DECLARATIONS*****
	
void initLcd(void);
void writeLcd(s8 word,s16 dc);
void moveLcdCursor(s16 row,s16 col);
void makeNewCharAt(const s8 *pixels,s16 asc);

void char2Lcd(s8 ch);
void str2Lcd(const s8 *str);

void clearLcdDisplay(void);
void clearLcdRow(s16 row);

//special
void scrllStr2Lcd(s8 *str,s16 row);

#endif

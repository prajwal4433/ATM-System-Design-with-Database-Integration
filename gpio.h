#ifndef _GPIO_H
#define _GPIO_H

#include "types.h"

#define OUT 1
#define IN  0

//GPIO macros
#define GPIO0 ((vu32 *)0xE0028000)
#define GPIO1 ((vu32 *)0xE0028010)

#define INITPIN(portAdd,pin,dir)		(*(portAdd+2)=(((*(portAdd+2))&~(1<<pin))|(dir<<pin)))
#define SETPIN(portAdd,pin)			 	(*(portAdd+1)= (1<<pin))
#define CLRPIN(portAdd,pin)		  	 	(*(portAdd+3)= (1<<pin))
#define READPIN(portAdd,pin)	     	(((*portAdd)>>pin)&1)
#define TGLPIN(portAdd,pin)    			((*portAdd)	^=(1<<pin))
#define WRITEPIN(portAdd,pin,val)    	(*portAdd=(((*portAdd)&~(1<<pin))|(val<<pin)))

#define INIT4PINS(portAdd,pin,dir)		(*(portAdd+2)=(((*(portAdd+2))&~(0xF<<pin))|(dir<<pin)))
#define READ4PINS(portAdd,pin)	     	(((*portAdd)>>pin)&0xF)
#define WRITE4PINS(portAdd,lpin,data) 	((*portAdd)=(((*portAdd)&~(0xF<<lpin))|(data<<lpin)))

#define INIT8PINS(portAdd,pin,dirVal) 	(*(portAdd+2)=(((*(portAdd+2))&~(0xFF<<pin))|(dirVal<<pin)))
#define SET8PINS(portAdd,lpin)			(*(portAdd+1)= (0xFF<<lpin))
#define CLR8PINS(portAdd,lpin)			(*(portAdd+3)= (0xFF<<lpin))
#define READ8PINS(portAdd,pin)	     	(((*portAdd)>>pin)&0xFF)
#define WRITE8PINS(portAdd,lpin,data) 	(*portAdd=(((*portAdd)&~(0xFF<<lpin))|(data<<lpin)))


#endif

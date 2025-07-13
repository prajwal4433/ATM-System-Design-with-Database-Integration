//MAIN BANK FILES
#include<stdio.h>
#include<stdio_ext.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include"bank.h"

int main()
{
	Acc *db=NULL,*from=NULL,*to=NULL;
	char key='\0',*pass=NULL,*usr=NULL;
	int bye=0;

	//create data folder is not present
	mkdir("../data", 0777);
	syncData(&db);
	puts("Hello All!!");
	puts("");
	while(1)
	{
		//wait for login
		loginMenu();
		printf("Enter Username:");
		usr=getStr();
		printf("Enter Password:");
		pass=getStr();

		if(!strcmp(usr,ADMIN_USRN))
		{
			if(!strcmp(pass,ADMIN_PASS))key='A';
			else if(!strcmp(pass,ADMIN_EXIT))exit(1);
		}
		else if(from=isValid(db,usr,pass)) 
			key='C';
		else
		{
			puts("Invalid credentials!");
			continue;
		}
		free(usr);free(pass);	

		if(key=='A')
		{
			bye=0;
			while(1)
			{
				adminMenu();
				key=getKey();
				if(!db)
				{
					if((key=='C')||(key=='Q'));
					else
					{
						puts("Empty DataBase!!");
						continue;
					}
				}

				if(key=='H'||key=='W'||key=='D'||key=='T'||key=='B'||key=='F'||key=='U'||key=='X')
				{
					puts("=== Enter Customer/Sender Info ===");
					from=getAcc(db);
					if(!from){
						puts("Not found!! Try again.");
						continue;
					}

				}
				switch(key)
				{
					case 'C':newAcc(&db);
						 break;
					case 'U':updateAcc(&db,from);
						 break;
					case 'H':statement(from);
						 break;
					case 'W':withdraw(from);
						 break;
					case 'D':deposit(from);
						 break;
					case 'B':balance(from);
						 break;
					case 'X':
						 if(from->cardStat !=1)
						 {
							 puts("Card status :BLOCKED");
							 puts("Activate card?(y/n)");
							 key=getKey();
							 if(key=='Y')
							 {
								 from->cardStat=1;
								 puts("card is activated.");
							 }
						 }
						 else
						 {
							 puts("Card status :ACTIVE");
							 puts("de-Activate card?(y/n)");
							 key=getKey();
							 if(key=='Y')
							 {
								 from->cardStat=0;
								 puts("card is blocked.");
							 }
						 }
						 break;
					case 'T':puts("==:Enter Receiver's info:==");
						 to=getAcc(db);
						 if(!to)
						 {
							 puts("Not found!! Try again.");
							 break;
						 }
						 transfer(from,to);
						 break;
					case 'E':database(db);
						 break;
					case 'F':dispAcc(from);
						 break;
					case 'Q':saveData(db);
						 bye=1;
						 break;
					default :puts("invalid option!.");
						 break;
				}
				if(bye)
				{
					puts("Thank you for your work,Admin <3");
					break;
				}
			}
		}
		else if(key=='C')
		{
			bye=0;
			while(1)
			{
				userMenu();
				key=getKey();
				switch(key)
				{
					case 'H':statement(from);
						 break;
					case 'W':withdraw(from);
						 break;
					case 'D':deposit(from);
						 break;
					case 'B':balance(from);
						 break;
					case 'T':puts("==:Enter Receiver's info:==");
						 to  =getAcc(db);
						 if(!to)
						 {
							 puts("Not found!! Try again.");
							 break;
						 }
						 transfer(from,to);
						 break;
					case 'Q':saveData(db);
						 bye=1;
						 break;
					default :puts("invalid option!.");
						 break;
				}
				if(bye)
				{
					puts("bye");
					break;
				}

			}
		}

	}
}



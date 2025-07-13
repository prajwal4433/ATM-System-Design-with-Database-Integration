//BANK FUNCTION FILES 
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "bank.h"
#include <termios.h>
#include <fcntl.h>

const int szDb=(sizeof(u64)*2+sizeof(f64)+sizeof(char)*(MAX_USRN_LEN+MAX_PASS_LEN));

void loginMenu(void)
{
	printf("\n-------------WELCOME TO VECTOR BANK--------------\n");
	printf("Please enter login credentials.\n");
}

void adminMenu(void)
{
	printf("\nHI ADMIN:\n"
			"[KEY]  : ACTION\n"
			"c/C    : Create New account.\n"
			"u/U    : Update Existing account.\n"
			"h/H    : Transaction history.\n"
			"w/W    : Withdraw amount.\n"
			"d/D    : Deposit amount.\n"
			"b/B    : Balance enquery.\n"
			"t/T    : Transfer money.\n"
			"x/X    : Activate card.\n"
			"e/E    : Display all accounts details.\n"
			"f/F    : Finding/searching for specific account.\n"
			"q/Q    : Quit from app.\n"
			"Enter choice:");
}

void userMenu(void)
{
	printf("\nHI CUSTOMER:\n"
			"[KEY]  : ACTION\n"
			"h/H    : Transaction history.\n"
			"w/W    : Withdraw amount.\n"
			"d/D    : Deposit amount.\n"
			"b/B    : Balance enquery.\n"
			"t/T    : Transfer amount.\n"
			"q/Q    : Quit from app.\n"
			"Enter choice:");

}

void menu(void)
{
	printf("\n------------------MENU--------------------------\n"
			"[KEY]	: ACTION\n"
			"c/C 	: Create New account.\n"
			"u/U    : Update Old account info.\n"
			"h/H 	: Transaction history.\n"
			"w/W 	: Withdraw amount.\n"
			"d/D 	: Deposit amount.\n"
			"b/B	: Balance enquery.\n"
			"t/T 	: Transfer money.\n"
			"e/E	: Display all accounts details.\n"
			"f/F	: Finding/searching for specific account.\n"
			"q/Q	: Quit from app.\n"
			"Enter choice:");
}

Acc* isValid(Acc *head,char *usr,char *pass)
{
	while(head)
	{
		if((!strcmp(usr,head->usrName))&&(!strcmp(pass,head->pass)))
			break;
		head=head->nxt;
	}
	return head;
}

void newAcc( Acc **head)
{
	char *temp=NULL,flag=1;
	double d;
	Acc *new=calloc(1,sizeof(Acc));
	new->tranHist=NULL;
	new->tranCnt=0;
	new->num=getUnqId(*head);
	printf("Enter Name:");
	__fpurge(stdin);
	new->name=getStr();
	if(strlen(new->name)<3)
	{
		puts("no name");
		free(new->name);
		free(new);
		return;
	}
	format(new->name);

	flag=1;
	while(flag)
	{
		if(flag!=1)puts("Mobile must be 10 digit, and min:60000 0000"); 
		printf("Enter Mobile No.:");	
		scanf("%llu",&(new->phno));
		if((new->phno>=6000000000)&&(new->phno<9999999999))
		{
			break;
		}else
		{
			flag++;
		}
	}

	flag=1;
	while(flag)
	{
		printf("Enter Login Username:");
		temp=getStr();
		if(strlen(temp)>=MAX_USRN_LEN)
		{
			puts("Username too long,try again!!");
			free(temp);
			continue;
		}
		if(isUnq(*head,temp))
		{
			strcpy(new->usrName,temp);
			free(temp);
			flag=0;
		}else
		{
			puts("User name aldready exits!");
			puts("Try differnt one.");
			free(temp);
		}

	}
	flag=1;
	while(flag)
	{
		if(flag>1)puts("Password mismatch!! Retry.");
		printf("Enter Login Password:");
		temp=getStr();
		if(strlen(temp)>=MAX_PASS_LEN)
		{
			puts("Password too long,try again!!");
			flag=1;
			free(temp);
			continue;
		}
		strcpy(new->pass,temp);
		free(temp);
		printf("Re-enter Login Password:");
		temp=getStr();
		flag=(strcmp(temp,new->pass)?flag+1:0);
	}
	free(temp);
	flag=1;
	while(flag)
	{
		if(flag>1)printf("8 digit RFID please:");
		else printf("Enter RFID card number:");

		temp=getStr();
		if(strlen(temp)!=8)
		{
			puts("try again!!");
			flag++;
			free(temp);
			continue;
		}else
		{
			if(isNewRFID(*head,temp))
			{
				strcpy(new->rfid,temp);
				free(temp);
				break;
			}else
			{
				puts("RFID aldready in use!!");
				flag++;
				free(temp);
			}
		}
	}
	flag=1;
	while(flag)
	{
		if(flag>1)
			puts("Pin mismatch!! Retry.");
		printf("Enter ATM Pin:");
		temp=getStr();
		if(strlen(temp)!=4)
		{
			puts("4 digit Pin please,try again!!");
			flag=1;
			free(temp);
			continue;
		}
		strcpy(new->pin,temp);
		free(temp);
		printf("Re-enter ATM Pin:");
		temp=getStr();
		flag=(strcmp(temp,new->pin)?flag+1:0);
	}
	free(temp);
	new->cardStat=1; //active the card 
	flag=1;
	while(flag)
	{
		if(flag>1)
			printf("Enter an posivite amount:");
		else 
			printf("Enter Opening Amount:");
		scanf("%lf",&d); //must update trancs
		if(d>0)
		{
			new->bal=d;
			break;
		}
		else 
		{
			flag++;
		}
	}
	addTran(new,new->bal,DEPOSIT);//0

	dispAcc(new);
	//add to database
	new->nxt=*head;
	*head=new;
	puts("Account Created.");

}

int isUnq(Acc *head,char *str)
{
	if(!strcmp(str,ADMIN_USRN)) 
		return 0;
	while(head)
	{
		if(!strcmp(str,head->usrName)) 
			return 0;
		head=head->nxt;
	}
	return 1;
}

int isNewRFID(Acc *head,char *rf)
{
	while(head)
	{
		if(!strcmp(rf,head->rfid)) 
			return 0;
		head=head->nxt;
	}
	return 1;
}

u64 getUnqId(Acc *head)
{
	u64 num;
	Acc *temp=NULL;
	int found=1;
	while(found)
	{
		srand(getpid()+ (unsigned int)head);
		//18 digit unq ID	
		num=getTimeStamp()*10000 +(rand()%10000);
		temp=head;
		found=0;
		while(temp)
		{
			if(num==temp->num)
			{
				found=1;
				break;
			}
			temp=temp->nxt;
		}
	}
	return num;
}

u64 getTimeStamp(void)
{
	time_t rawtime;
	struct tm *timeinfo;

	// Get current UTC time
	time(&rawtime);

	// Convert to IST (UTC +5:30)
	// rawtime +=19800;  // 19800 seconds = 5 hours 30 minutes
	timeinfo =localtime(&rawtime);

	// Format as YYYYMMDDHHMMSS (14-digit ID)
	u64 timeStamp =
		(timeinfo->tm_year+1900)*10000000000ULL+
		(timeinfo->tm_mon+1)*100000000ULL+
		timeinfo->tm_mday*1000000ULL+
		timeinfo->tm_hour*10000ULL+
		timeinfo->tm_min *100ULL+
		timeinfo->tm_sec;

	return timeStamp;
}

char *getStr(void)
{
	char buff[NAME_LEN]={0};
	fflush(stdout);
	__fpurge(stdin);
	scanf("%[^\n]",buff);
	return strdup(buff);
}

char getch(void)
{
	struct termios oldt, newt;
	int oldf;
	int ch;

	// Save terminal settings
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	// Disable canonical mode and echo
	newt.c_lflag &= ~(ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	// Set non-blocking mode
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	// Restore settings
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		return ch;
	}

	return -1; // No input
}

char getKey(void)
{
	char key[3],ch;
	int i=0;
	fflush(stdout);
	__fpurge(stdin);
	key[0]=getchar();
	CAPS(key[0]);
	puts("");
	return key[0];
}
void format(char *str)
{
	CAPS(str[0]);
	str++;
	while(*str)
	{
		if(*(str-1)==' ')CAPS(*str);
		str++;
	}
}
void updateAcc(Acc **head,Acc *usr)
{
	char key,*temp=NULL;
	int flag=1;
	accMenu();

	key=getKey();
	__fpurge(stdin);
	puts("");
	switch(key)
	{
		case 'P':printf("Enter New phone number:");
			 scanf("%llu",&(usr->phno));
			 puts("Mobile Updated.");
			 break;
		case 'O':printf("Enter New Holder name:");
			 __fpurge(stdin);
			 temp=getStr();
			 if(strlen(temp)<3)
			 {
				 puts("invalid name");
				 free(temp);
				 return;
			 }
			 format(temp);
			 usr->name=temp;
			 puts("Name Updated.");
			 break;
		case 'U':flag=1;
			 while(flag)
			 {
				 printf("Enter New Username:");
				 temp=getStr();
				 if(strlen(temp)>MAX_USRN_LEN)
				 {
					 puts("Username too long,try again!!");
					 continue;
				 }
				 if(!strcmp(temp,usr->usrName))
				 {
					 puts("New Username is same as your Existing Username.");
					 puts("Do you wanna change the username?(y/n):");
					 key=getKey();
					 if(key=='Y')
					 {
						 free(temp);
						 continue;
					 }
					 else if(key=='N')
					 {
						 free(temp);
						 return;
					 }

				 }
				 else if(isUnq(*head,temp))
				 {
					 strcpy(usr->usrName,temp);
					 free(temp);
					 flag=0;
				 }
				 else
				 {
					 puts("User name aldready exits!");
					 puts("Try differnt one.");
				 }
			 }
			 if(!flag)
				 puts("Username Updated.");
			 break;
		case 'Q':printf("Enter old password:");
			 temp=getStr();
			 if(strcmp(temp,usr->pass))
			 {
				 puts("Wrong password");
				 break;
			 }
			 flag=1;
			 while(flag)
			 {
				 if(flag>1)
					 puts("Password mismatch Retry.");
				 printf("Enter New Login Password:");
				 temp=getStr();
				 if(strlen(temp)>=MAX_PASS_LEN)
				 {
					 puts("Password too long,try again!!");
					 flag=1;
					 continue;
				 }
				 strcpy(usr->pass,temp);
				 free(temp);
				 printf("Re-enter New Login Password:");
				 temp=getStr();
				 flag=(strcmp(temp,usr->pass)?flag+1:0);
			 }
			 puts("Password Updated.");
			 break;
		case 'A':printf("Enter Old ATM Pin:");
			 temp=getStr();
			 if(strcmp(temp,usr->pin))
			 {
				 puts("Wrong pin!!");
				 free(temp);
				 break;
			 }
			 flag=1;
			 free(temp);
			 while(flag)
			 {
				 if(flag>1)puts("Pin mismatch!! Retry.");
				 printf("Enter New ATM Pin:");
				 temp=getStr();
				 if(strlen(temp)!=4)
				 {
					 puts("4 digit pin pls,try again!!");
					 flag=1;
					 free(temp);
					 continue;
				 }
				 strcpy(usr->pin,temp);
				 free(temp);
				 printf("Re-enter New ATM Pin:");
				 temp=getStr();
				 flag=(strcmp(temp,usr->pin)?flag+1:0);
				 free(temp);
			 }
			 puts("Pin Updated.");
			 break;
		default:puts("invalid input.");
			break;
	}
}

void accMenu(void)
{
	printf("\nChange:\n"
			"[KEY]-ACTION\n"
			"p/P  -Phone number.\n"
			"o/O  -Holder's name.\n"
			"u/U  -Username.\n"
			"q/Q  -Password.\n"
			"Enter choice:");
}

void dispAcc(Acc *usr)
{
	puts("\n==:Account Details:==");
	printf("AccNo.:%llu\n",usr->num);
	printf("Name  :%s\n",usr->name);
	printf("Ph.No.:%llu\n",usr->phno);
	printf("Balanc:%lf\n",usr->bal);
	printf("Usrnam:%s\n",usr->usrName);
	printf("Passwd:%s\n",usr->pass);
	printf("RFID  :%s\n",usr->rfid);
	printf("Pin   :%s\n",usr->pin);
	printf("Card  :%s\n",(usr->cardStat==1)?"ACTIVE":"BLOCKED");
	printf("TranNo:%llu\n",usr->tranCnt);
}

Acc* getAcc(Acc *head)
{
	u64 num=0;
	char key='\0',*str=NULL;
	printf( "Find by:\n"
			"[KEY]-ACTION\n"
			"p/P  -Phone number.\n"
			"n/N  -Account number.\n"
			"o/O  -Holder name.\n"
			"u/U  -Username.\n"
			"Enter choice:");
	key=getKey();
	__fpurge(stdin);
	puts("");
	switch(key)
	{
		case 'P':printf("Enter phone number:");
			 scanf("%llu",&num);
			 while(head)
			 {
				 if(num==head->phno)break;
				 head=head->nxt;
			 }
			 break;
		case 'N':printf("Enter account number:");
			 scanf("%llu",&num);
			 while(head)
			 {
				 if(num==head->num)break;
				 head=head->nxt;
			 }
			 break;
		case 'O':printf("Enter holder name:");
			 str=getStr();
			 format(str);
			 while(head)
			 {
				 if(!strcmp(str,head->name))break;
				 head=head->nxt;
			 }
			 free(str);
			 break;
		case 'U':printf("Enter username:");
			 str=getStr();
			 while(head)
			 {
				 if(!strcmp(str,head->usrName))break;
				 head=head->nxt;
			 }
			 free(str);
			 break;
		default:puts("invalid choice");
			head=NULL;
			break;
	}
	return head;
}

void dltAcc(Acc **head)
{
	if(!(*head))return;
}

void balance(Acc *usr)
{
	printf("\nAccount Number : %llu\n",usr->num);
	printf("Holder Name    : %s\n",usr->name);
	printf("Current Balance: %+lf Rs/-\n",usr->bal);
}

void deposit(Acc *usr)
{
	f64 amt=0;
	printf("\nEnter Deposit Amount:");
	scanf("%lf",&amt);

	if(amt<=0){
		puts("Amount cannot be negative!!");
		puts("Try again!!");
	}
	else if(amt<MAX_DEPOSIT)
	{
		usr->bal += amt;
		//update 2 transc
		addTran(usr,+amt,DEPOSIT);
		puts("Amount Deposited.");
	}
	else
	{
		puts("Amount exceeds Max.Deposit limit!!");
		puts("Try again!!");
	}
}

void withdraw(Acc *usr)
{
	f64 amt=0;
	printf("\nEnter Withdrawal Amount:");
	scanf("%lf",&amt);
	if(amt<=0)
	{
		puts("Amount cannot be negative!!");
		puts("Try again!!");
	}
	else if(amt<MAX_WITHDRAW)
	{
		if(amt<=usr->bal)
		{
			usr->bal -= amt;
			//update 2 transc
			addTran(usr,-amt,WITHDRAW);//1
			puts("Amount Withdrawn.");
		}
		else
		{
			puts("Low Balance!!");
		}
	}
	else
	{
		puts("Amount exceeds Max.Withdraw limit!!");
		puts("Try again!!");
	}
}

void transfer(Acc *from,Acc *to)
{
	f64 amt=0;
	printf("\nEnter Transfer Amount:");
	scanf("%lf",&amt);
	if(amt<=0)
	{
		puts("Amount cannot be negative!!");
		puts("Try again!!");
	}
	else if(amt<MAX_TRANSFER)
	{
		if(amt<=from->bal)
		{
			from->bal -= amt;
			to->bal   += amt;
			//update 2 transc of both
			addTran(to,+amt,TRANSFER_IN);
			addTran(from,-amt,TRANSFER_OUT);
			puts("Amount Transfered.");
		}
		else
		{
			puts("Low Balance!!");
		}
	}
	else
	{
		puts("Amount exceeds Max.Transfer limit!!");
		puts("Try again!!");
	}
}

void addTran(Acc *usr,f64 amt,char type)
{
	Tran *new=calloc(1,sizeof(Tran));
	new->amt=amt;
	new->id =getTranId(usr);
	new->type=type;
	new->nxt=NULL;

	new->nxt=usr->tranHist;
	usr->tranHist=new;

	(usr->tranCnt)++;
}

u64 getTranId(Acc *usr)
{
	//17 digit unq TranID
	srand(usr->num);
	return getTimeStamp()*1000 +(rand()%1000);
}

void statement(Acc *usr)
{
	Tran *temp = usr->tranHist;
	if(temp)
	{
		printf("\n%-20s%-23s%-12s\n", "Transaction ID", "Amount (Rs)","Type");
		puts("----------------------------------------");
		while (temp) {
			printf("%-20llu%-+20.2lf",temp->id,temp->amt);
			if(temp->type==DEPOSIT)	 printf("%-12s\n","Deposit");
			else if(temp->type==WITHDRAW)printf("%-12s\n","Withdraw");
			else if(temp->type==TRANSFER_IN)printf("%-12s\n","Tranfer IN");
			else if(temp->type==TRANSFER_OUT)printf("%-12s\n","Tranfer OUT");
			temp = temp->nxt;
		}
	} 
	else 
	{
		puts("No Transaction History!");
	}
}

void database(Acc *head)
{
	if(!head)
	{
		puts("Empty Database!!WTF");
		return;
	}
	printf("\n%-20s|%-40s|%-14s|%-12s\n",
			"Account ID",
			"Holder Name",
			"Mobile(+91)",
			"Transactions");
	while(head)
	{
		printf("%-20llu|%-40s|+91-%-10llu|%-12llu\n",
				head->num,
				head->name,
				head->phno,
				head->tranCnt);
		head=head->nxt;
	}
}
void saveData(Acc *head)
{
	// Create data directory if it doesn't exist
	struct stat st = {0};
	if(stat("../data", &st) == -1 && mkdir("../data", 0700) == -1)
	{
		perror("Failed to create data directory");
		return;
	}

	// Open users file
	FILE *userFile = fopen("../data/users.txt", "w");
	if (!userFile) {
		perror("Failed to open users file");
		return;
	}

	// Open transactions file
	FILE *transFile = fopen("../data/transactions.txt", "w");
	if (!transFile) {
		perror("Failed to open transactions file");
		fclose(userFile);
		return;
	}

	// Write headers
	fprintf(userFile, "RFID,PIN,Name,Balance,AccountNumber,Phone,Username,CardStatus,TransactionCount\n");
	fprintf(transFile, "RFID,Type,Amount,DateTime,TransactionID\n");

	// Write all user data
	Acc *current = head;
	while (current) {
		// Ensure all string fields are properly initialized
		if (current->name == NULL) current->name = strdup("");
		if (current->usrName[0] == '\0') strncpy(current->usrName, "default", MAX_USRN_LEN-1);
		if (current->rfid[0] == '\0') strncpy(current->rfid, "00000000", 8);
		if (current->pin[0] == '\0') strncpy(current->pin, "0000", 4);

		// Write user record
		fprintf(userFile, "%s,%s,%s,%.2f,%llu,%llu,%s,%d,%llu\n",
				current->rfid, current->pin, current->name, current->bal,
				current->num, current->phno, current->usrName,
				current->cardStat, current->tranCnt);

		// Write all transactions for this user
		Tran *t = current->tranHist;
		while (t) {
			time_t rawtime = time(NULL);
			struct tm *timeinfo = localtime(&rawtime);

			fprintf(transFile, "%s,%d,%.2f,%04d-%02d-%02d %02d:%02d:%02d,%llu\n",
					current->rfid, t->type, t->amt,
					timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
					timeinfo->tm_mday, timeinfo->tm_hour,
					timeinfo->tm_min, timeinfo->tm_sec,
					t->id);

			t = t->nxt;
		}
		current = current->nxt;
	}

	// Cleanup
	fclose(userFile);
	fclose(transFile);
}

void syncData(Acc **head)
{
	// Initialize output
	*head = NULL;
	Acc *tail = NULL;

	// Open users file
	FILE *userFile = fopen("../data/users.txt", "r");
	if (!userFile) {
		perror("Failed to open users file");
		return;
	}

	char line[512];
	// Skip header line
	if (!fgets(line, sizeof(line), userFile)) {
		fclose(userFile);
		return;
	}

	// Read user records
	while (fgets(line, sizeof(line), userFile)) {
		// Allocate new user
		Acc *newUser = calloc(1, sizeof(Acc));
		if (!newUser) {
			perror("Memory allocation failed");
			continue;
		}

		// Initialize all fields
		newUser->name = calloc(NAME_LEN, sizeof(char));
		if (!newUser->name) {
			free(newUser);
			continue;
		}

		// Parse user record with proper type specifiers
		if (sscanf(line, "%8[^,],%4[^,],%[^,],%lf,%llu,%llu,%19[^,],%d,%llu",
					newUser->rfid, newUser->pin, newUser->name, &newUser->bal,
					&newUser->num, &newUser->phno, newUser->usrName,
					&newUser->cardStat, &newUser->tranCnt) != 9) {
			free(newUser->name);
			free(newUser);
			continue;
		}

		// Initialize transaction history
		newUser->tranHist = NULL;
		newUser->nxt = NULL;

		// Add to linked list
		if (!*head) {
			*head = newUser;
		} else {
			tail->nxt = newUser;
		}
		tail = newUser;
	}
	fclose(userFile);

	// Load transactions if users were loaded successfully
	if (*head) {
		FILE *transFile = fopen("../data/transactions.txt", "r");
		if (!transFile) {
			return;
		}

		// Skip header
		if (!fgets(line, sizeof(line), transFile)) {
			fclose(transFile);
			return;
		}

		// Read transaction records
		while (fgets(line, sizeof(line), transFile)) {
			char rfid[9];
			int type;
			double amount;
			char datetime[20];
			u64 transId;

			if (sscanf(line, "%8[^,],%d,%lf,%19[^,],%llu",
						rfid, &type, &amount, datetime, &transId) != 5) {
				continue;
			}

			// Find the user for this transaction
			Acc *user = *head;
			while (user && strcmp(user->rfid, rfid)) {
				user = user->nxt;
			}

			if (!user) continue;

			// Create new transaction
			Tran *newTrans = calloc(1, sizeof(Tran));
			if (!newTrans) continue;

			newTrans->type = type;
			newTrans->amt = amount;
			newTrans->id = transId;
			newTrans->nxt = user->tranHist;
			user->tranHist = newTrans;
		}
		fclose(transFile);
	}
}


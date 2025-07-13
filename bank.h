// bankers header file

#ifndef _BANK_H_
#define _BANK_H_

#define MAX_DEPOSIT	30000 //30000rs
#define MAX_WITHDRAW	30000 //30000rs
#define MAX_TRANSFER 	100000//100000rs

#define NAME_LEN 30
#define MAX_PASS_LEN 20
#define MAX_USRN_LEN 20

#define ADMIN_USRN "admin"
#define ADMIN_PASS "admin"
#define ADMIN_EXIT "exit"

#define WITHDRAW 1
#define DEPOSIT  2
#define TRANSFER_IN 3
#define TRANSFER_OUT 4

#define CAPS(ch) (ch &=~(32))

typedef unsigned long long int u64;
typedef double f64;

typedef struct A{
	f64 amt;
	u64 id;
	char type;

	struct A *nxt;
}Tran;

typedef struct B{
	u64 num;//unique acc num/id
	f64 bal;//bank balance
	u64 phno;//mobile
	char usrName[MAX_USRN_LEN];
	char pass[MAX_PASS_LEN];
	char rfid[9];
	char pin[5];
	int cardStat;//1-active,0-block cuz wrong login,2-block during pin change
	char *name;//name

	Tran *tranHist;
	u64 tranCnt;
	struct B *nxt;
}Acc;

extern const int szDb;

//functions
//Account-handling
//main
void loginMenu(void);
void adminMenu(void);
void userMenu(void);
void accMenu(void);
void menu(void);

Acc* isValid(Acc*,char*,char*);
int isUnq(Acc*,char*);
int isNewRFID(Acc *head,char *rf);
void dispAcc(Acc*);

void newAcc(Acc**);
void updateAcc(Acc**,Acc*);
void dltAcc(Acc**);
Acc* getAcc(Acc*);

void withdraw(Acc*);
void deposit(Acc*);
void transfer(Acc*,Acc*);
void addTran(Acc*,f64,char);
void balance(Acc*);
void statement(Acc*);
void database(Acc*);

//sub
u64   getUnqId(Acc*);
u64   getTranId(Acc*);
u64   getTimeStamp(void);
char  getKey(void);
char* getStr(void);
void  format(char*);

//file-handling
void saveData(Acc *);
//void saveFile(Acc *head);
void syncData(Acc **);

#endif


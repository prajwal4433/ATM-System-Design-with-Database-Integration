
bank:bankMain.o bank.o
	cc bankMain.o bank.o -o bank
bankMain.o:bankMain.c
	cc -c bankMain.c
bank.o:bank.c
	cc -c bank.c


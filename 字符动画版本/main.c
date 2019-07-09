#include <stdlib.h>
#include <stdio.h>
#include <windows.h> 
#include "define.h"
#define TICK 2
#define DEFAULT_BALANCE 1000

int OrderCnt=0;
int CurrentTime=0;
Rider *RiderListHead;
Rider *LastRider;

Order *OrderListHead;

Node *OrderOverdueList;
Node *LastOrderOverdue;
Node *OrderCompletedList;
Node *LastOrderCompleted;

int RiderCnt=0;
int OrderCnt;
int OrderOverdue=0;
int OrderCompleted=0;
int OrderTaken=0;
int Balance=DEFAULT_BALANCE;


void init();
void clear();
void debug_printProcess(char *str);

extern int salesInput();
//from file.c
extern int isEnd();
//from strategy.c
extern void updateAllRiders();
//from strategy.c
extern void assignOrderPeriodic();
//from strategy.c
extern void updateFile();
//from file.c
extern void updateScreen();
//from display.c
extern void splash();
//from display.c
extern void sortOrder();
//from file.c
extern void printScreen();
//from display.c
extern void debug_printOrders();
//from file.c

int main(int argc, char const *argv[])
{
	splash();
	init();
	OrderCnt=salesInput();
	sortOrder(OrderListHead,OrderCnt);
	displayAllElems();
	while(!isEnd())
	{
		updateAllRiders();
		assignOrderPeriodic(CurrentTime);
		updateFile();
		updateScreen();
		printScreen();
		CurrentTime++;
		Sleep(200);
	}
	clear();
	system("PAUSE");
	return 0;
}

void init()
{
	RiderListHead=(Rider*)malloc(sizeof(Rider));
	RiderListHead->nextPtr=NULL;

	OrderListHead=(Order*)malloc(sizeof(Order));
	OrderListHead->nextPtr=NULL;

	OrderOverdueList=(Node*)malloc(sizeof(Node));
	OrderOverdueList->nextPtr=NULL;

	OrderCompletedList=(Node*)malloc(sizeof(Node));
	OrderCompletedList->nextPtr=NULL;
	LastRider=NULL;
	LastOrderOverdue=NULL;
	LastOrderCompleted=NULL;
}

void clear()
{
	Rider *tmpRider;
	while(RiderListHead)
	{
		tmpRider=RiderListHead;
		RiderListHead=RiderListHead->nextPtr;
		free(tmpRider);
	}
	//骑手链表被释放

	Node *tmpPtr;
	while(OrderOverdueList)
	{
		tmpPtr=OrderOverdueList;
		OrderOverdueList=OrderOverdueList->nextPtr;
		free(tmpPtr);
	}
	OrderOverdueList=NULL;
	LastOrderOverdue=NULL;
	//超时清单被释放

	while(OrderCompletedList)
	{
		tmpPtr=OrderCompletedList;
		OrderCompletedList=OrderCompletedList->nextPtr;
		free(tmpPtr);
	}
	OrderCompletedList=NULL;
	LastOrderCompleted=NULL;
	//完成清单被释放
}


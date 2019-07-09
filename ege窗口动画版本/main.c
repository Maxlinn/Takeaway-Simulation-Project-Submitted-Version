#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "define.h"
#include "graphics.h"
#include <mmsystem.h> 
#define TICK 200//֡����ʱ�䣬Ĭ��Ϊ200ms 
#define DEFAULT_BALANCE 1000//Ĭ����� 

int CurrentTime=0;
Rider *RiderListHead;//����������ڱ�ͷ���
Rider *LastRider;//���������β���

Order *OrderListHead;//����������ڱ�ͷ���
Order *LastOrder;

Queue OverdueQueue;
Queue CompletedQueue;

int RiderCnt=0;
int OrderCnt=0;
int OrderOverdue=0;
int OrderCompleted=0;
int OrderTaken=0;
int Balance=DEFAULT_BALANCE;
int input_flag;
int output_flag;
enum INPUT_FLAG
{
	FILEINPUT,ANIMATEINPUT
};

enum OUTPUT_FLAG
{
	FILEOUTPUT,ANIMATEOUTPUT,BOTHOUTPUT
};

void init();
//�����������ն���
extern void welcome();
//from animate.c
//���ܣ�����input_flag��output_flag;
extern void loadPics();
//from animate.c
//���ܣ����빫��ͼƬ��Դ��animate��
extern void fileInput();
//from file.c 
extern int isEnd();
//from strategy.c
extern void updateAllRiders();
//from strategy.c
extern void printScreen();
//from animate.c
extern void updateScreen();
//from animate.c
extern void assignOrderPeriodic(int);
//from strategy.c
extern void updateFile();
//from file.c
extern void mouseDealer();
//from animate.c
void sortOrder(Order *OrderListHead,int n);
//from file.c
extern void debug_printRiders();
//from file.c
extern void debug_printOrders();
//from file.c
extern void debug_printMap();
//from file.c
extern Queue createEmptyQueue();
//from file.c
extern void broken();
//from animate.c

int main(int argc, char const *argv[])
{
	init();
	//�½����ֲ����߳�
	PlaySound(".\\assets\\Foxtail-Grass Studio - ������ñ�Ӥȷ����, Unauthorized Compressed with Au.wav",NULL,SND_LOOP|SND_ASYNC);
	welcome();
	loadPics();
	
	HANDLE hThread1;
	if(input_flag==FILEINPUT) 
	{
		fileInput();
		sortOrder(OrderListHead,OrderCnt);
	}
	else//ֻ�ڶ����汾����������߳� 
	{
		DWORD hThreadID1;
		hThread1=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)mouseDealer,NULL,0,&hThreadID1);
	}
	
	for(;!isEnd();delay_fps(120))
	{		
		updateAllRiders();
		assignOrderPeriodic(CurrentTime);
		updateFile();
		updateScreen();
		printScreen();
		CurrentTime++;
		Sleep(TICK);
	}
	
	if(Balance<0) 
	{
		broken();
		PlaySound(".\\assets\\����,¦���� - ����, Unauthorized Modified and Compressed with Au.wav",NULL,SND_LOOP|SND_ASYNC);
	}
	
	CloseHandle(hThread1);
	getch();
	closegraph();
	return 0;
}

void init()
{
	RiderListHead=(Rider*)malloc(sizeof(Rider));
	RiderListHead->nextPtr=NULL;

	OrderListHead=(Order*)malloc(sizeof(Order));
	OrderListHead->nextPtr=NULL;

	OverdueQueue=createEmptyQueue();
	CompletedQueue=createEmptyQueue();

	LastRider=NULL;
	LastOrder=NULL;
}

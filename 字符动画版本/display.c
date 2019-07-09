#include "define.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum PARAMETERS{
	RESOLUTION_X=220,RESOLUTION_Y=60,IMG_X=6,IMG_Y=3,
	LEFT_JUSTIFYING=0,MID_JUSTIFYING=1,RIGHT_JUSTIFYING=2,
	POLE_X=3*RESOLUTION_X/5,
	SPLASH_Y=30,SPLASH_X=80,
	DEFAULT_WATING_TIME=4,
};
//输出图像的参数
char screen[RESOLUTION_Y][RESOLUTION_X+1];
//图像存储的数组
//所有的坐标是行后列，X是横向伸展，Y是纵向伸展，所以看起来是先y后x

extern int isFileAvailable();
extern int OrderCnt;
extern Order *OrderListHead;
extern Rider *RiderListHead;
extern int CurrentTime;
extern int Balance;
extern int OrderTaken;
extern int OrderCompleted;
extern int OrderOverdue;

void initScreen();
void fillZone(int y1,int x1,int y2,int x2,char ch);
void drawStraightLine(int y1,int x1,int y2,int x2);
void printScreen();
void writeWord(char *src,int y1,int x1,int mode);
int xShift(int logic_x);
int yShift(int logic_y);
void pasteImg(Image from,int y,int x);
void drawRect(int y1,int x1,int y2,int x2);
void splash();
void displayAllElems();
void updateScreen();

Image Canteen={
	"+____+|Shop|======",IMG_X,IMG_Y
};
Image Customer={
	"+____+|Cust|======",IMG_X,IMG_Y
};
Image Normal={
	"+____+|    |======",IMG_X,IMG_Y
};
Image Road={
	"                  ",IMG_X,IMG_Y
};
Image Man={
	"      Rider       ",IMG_X,IMG_Y
};
Image Welcome={
	" __          __  _                           \\ \\        / / | |                           \\ \\  /\\  / /__| | ___ ___  _ __ ___   ___    \\ \\/  \\/ / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\    \\  /\\  /  __/ | (_| (_) | | | | | |  __/     \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___|",44,6
};

Order *OrderClipDisplay=NULL;
void initScreen()
{
	fillZone(0,0,RESOLUTION_Y-1,RESOLUTION_X-1,' ');
	fillZone(0,RESOLUTION_X,RESOLUTION_Y-1,RESOLUTION_X,'\0');
	//清理区域
	drawRect(0,0,RESOLUTION_Y-1,RESOLUTION_X-1);
	//画边框
	drawStraightLine(2,0,2,RESOLUTION_X-1);
	writeWord("Real-Time Takeaway Order Process Simulation",1,RESOLUTION_X/2,MID_JUSTIFYING);
	//画标题栏	
	drawStraightLine(2,POLE_X,RESOLUTION_Y-1,POLE_X);
}

void fillZone(int y1,int x1,int y2,int x2,char ch)
//用ch填充(y1,x1)和(y2,x2)组成的矩形区域（含边界），这两点不区分位置关系
{
	int low_x,low_y,high_x,high_y;
	if(y2<y1)
	{
		low_y=y2;high_y=y1;
		if(x2<x1) {low_x=x2;high_x=x1;}
		else {low_x=x1;high_x=x2;}
	}
	else
	{
		low_y=y1;high_y=y2;
		if(x2<x1) {low_x=x2;high_x=x1;}
		else {low_x=x1;high_x=x2;}
	}
	//找出遍历时的上界下界
	int i,j;
	for(i=low_y;i<=high_y;++i)
		for(j=low_x;j<=high_x;++j)
			screen[i][j]=ch;
}

void drawStraightLine(int y1,int x1,int y2,int x2)
{
	//在两点之间画直线，横的用-，竖的用|，交叉时用+
	//两点必须在同一行或同一列，但是相对位置没有限制
	int i;
	if(y1==y2)//在同一行
	{	
		int low_x=(x1<x2)?x1:x2;
		int high_x=(x1>x2)?x1:x2;
		for(i=low_x;i<=high_x;++i)
				screen[y1][i]=(screen[y1][i]=='|')?'+':'-';
	}
	else if(x1==x2) 
		{
			int low_y=(y1<y2)?y1:y2;
			int high_y=(y1>y2)?y1:y2;
			for(i=low_y;i<=high_y;++i)
					screen[i][x1]=(screen[i][x1]== '-')?'+':'|';
		}
	else return ;
}

void printScreen()
{
	int i;
	system("CLS");
	for(i=0;i<RESOLUTION_Y;++i)
		puts(screen[i]);
}

void drawRect(int y1,int x1,int y2,int x2)
{
	//画矩形边框
	int low_x,low_y,high_x,high_y;
	if(y2<y1)
	{
		low_y=y2;high_y=y1;
		if(x2<x1) {low_x=x2;high_x=x1;}
		else {low_x=x1;high_x=x2;}
	}
	else
	{
		low_y=y1;high_y=y2;
		if(x2<x1) {low_x=x2;high_x=x1;}
		else {low_x=x1;high_x=x2;}
	}
	//确定上下界
	drawStraightLine(low_y,low_x,low_y,high_x);
	drawStraightLine(low_y,high_x,high_y,high_x);
	drawStraightLine(high_y,high_x,high_y,low_x);
	drawStraightLine(high_y,low_x,low_y,low_x);
}

void writeWord(char *src,int y1,int x1,int mode)
{
	int i;
	switch(mode)
	{
		case LEFT_JUSTIFYING:break;
		case MID_JUSTIFYING:
			x1=x1-strlen(src)/2;break;
		case RIGHT_JUSTIFYING:
			x1=x1-strlen(src);break;
	}
	//确定x1的位置
	for(i=0;i<strlen(src);x1++,i++)
		screen[y1][x1]=src[i];
}

int xShift(int logic_x)
{
	//将位于[0,16]x[0,16]范围内的逻辑坐标转换成画图坐标的左上角
	return 4+IMG_X*logic_x;
}

int yShift(int logic_y)
{
	return 5+IMG_Y*logic_y;
}

void pasteImg(Image from,int y,int x)
{
	//即在[y,x]->[y+len_y,x+len_x]矩形区域填充图像
	int i,j;
	for(i=0;i<from.len_y;++i)
		for(j=0;j<from.len_x;++j)
				screen[y+i][x+j]=from.src[j+i*from.len_x];
}

void splash()
{
	initScreen();
	int i,j;
	for(i=0;i<=16;++i)
		for(j=0;j<=16;++j)
			if(i%2==0&&j%2==0)
				pasteImg(Normal,yShift(j),xShift(i));
			else
				pasteImg(Road,yShift(j),xShift(i));
	//贴背景图
	int lu_y=RESOLUTION_Y/2-SPLASH_Y/2;
	int lu_x=RESOLUTION_X/2-SPLASH_X/2;
	int rd_y=RESOLUTION_Y/2+SPLASH_Y/2;
	int rd_x=RESOLUTION_X/2+SPLASH_X/2;
	fillZone(lu_y,lu_x,rd_y,rd_x,' ');
	drawRect(lu_y,lu_x,rd_y,rd_x);
	pasteImg(Welcome,lu_y+1,lu_x+1);
	drawStraightLine(lu_y+Welcome.len_y+1,lu_x,lu_y+Welcome.len_y+1,rd_x);
	if(isFileAvailable())
	{
		writeWord("Detected sales.txt in current directory",RESOLUTION_Y/2,RESOLUTION_X/2,MID_JUSTIFYING);
		writeWord("Simulation will start after countdown...",RESOLUTION_Y/2+2,RESOLUTION_X/2,MID_JUSTIFYING);
	}
	else
	{
		writeWord("There is not sales.txt in current directory",RESOLUTION_Y/2,RESOLUTION_X/2,MID_JUSTIFYING);
		writeWord("The CommandInput will be activated after countdown",RESOLUTION_Y/2+2,RESOLUTION_X/2,MID_JUSTIFYING);
	}
	int countdown_time=DEFAULT_WATING_TIME;
	char countdown_str[]="Countdown: 0s";
	while(countdown_time--)
	{
		system("CLS");
		printScreen();
		countdown_str[11]=countdown_time+'0';
		writeWord(countdown_str,rd_y-1,rd_x-1,RIGHT_JUSTIFYING);
		sleep(1);//睡一会
	}
	initScreen();
	system("CLS");
}

void displayAllElems()
{
	int i,j;
	for(i=0;i<=16;++i)
		for(j=0;j<=16;++j)
			if(i%2==0&&j%2==0)//双偶数，是房子
				pasteImg(Normal,yShift(j),xShift(i));
			else
				pasteImg(Road,yShift(j),xShift(i));
	writeWord("Current Time: ",5,POLE_X+2,LEFT_JUSTIFYING);
	writeWord("Dealing details: ",7,POLE_X+2,LEFT_JUSTIFYING);
	writeWord("Balance: ",8,POLE_X+4,LEFT_JUSTIFYING);
	writeWord("Order Confirmed: ",9,POLE_X+4,LEFT_JUSTIFYING);
	writeWord("Order Completed: ",10,POLE_X+4,LEFT_JUSTIFYING);
	writeWord("Order Overdue: ",11,POLE_X+4,LEFT_JUSTIFYING);
	//写字
}

void updateScreen()
{
	int i,j;
	for(i=0;i<=16;++i)
		for(j=0;j<=16;++j)
			if(!(i%2==0&&j%2==0))
				pasteImg(Road,yShift(i),xShift(j));
	//清空道路上的骑手
	Rider *iterator=RiderListHead->nextPtr;
	while(iterator)
	{
		pasteImg(Man,yShift(iterator->pos->y),xShift(iterator->pos->x));
		iterator=iterator->nextPtr;
	}

	static int cnt_2=1;
	if(!OrderClipDisplay&&cnt_2<=OrderCnt) OrderClipDisplay=OrderListHead->nextPtr;
	while(OrderClipDisplay)
	{
		if(OrderClipDisplay->time>CurrentTime) break;
		if(OrderClipDisplay->time==CurrentTime)
		{
			++cnt_2;
			pasteImg(Canteen,yShift(OrderClipDisplay->src->y),xShift(OrderClipDisplay->src->x));
			pasteImg(Customer,yShift(OrderClipDisplay->dst->y),xShift(OrderClipDisplay->dst->x));
		}
		OrderClipDisplay=OrderClipDisplay->nextPtr;
	}

	char tmpStr[20];
	itoa(CurrentTime,tmpStr,10);
	fillZone(5,POLE_X+2+14,5,RESOLUTION_X-2,' ');
	writeWord(tmpStr,5,POLE_X+2+14,LEFT_JUSTIFYING);
	
	itoa(Balance,tmpStr,10);
	fillZone(8,POLE_X+4+9,8,RESOLUTION_X-2,' ');
	writeWord(tmpStr,8,POLE_X+4+9,LEFT_JUSTIFYING);
	
	itoa(OrderTaken,tmpStr,10);
	fillZone(9,POLE_X+4+17,9,RESOLUTION_X-2,' ');
	writeWord(tmpStr,9,POLE_X+4+17,LEFT_JUSTIFYING);

	itoa(OrderCompleted,tmpStr,10);
	fillZone(10,POLE_X+4+17,10,RESOLUTION_X-2,' ');
	writeWord(tmpStr,10,POLE_X+4+17,LEFT_JUSTIFYING);
	
	itoa(OrderOverdue,tmpStr,10);
	fillZone(11,POLE_X+4+15,11,RESOLUTION_X-2,' ');
	writeWord(tmpStr,11,POLE_X+4+15,LEFT_JUSTIFYING);
	
}
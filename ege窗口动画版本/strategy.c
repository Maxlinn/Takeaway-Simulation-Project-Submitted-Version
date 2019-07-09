#include "define.h"
#include <stdlib.h>
#include <stdio.h>

const int INF=9999999;
const int velocity=1;
const int Default_Born_Point_Y=8;
const int Default_Born_Point_X=9;
const int Penalty_Factor_Newbie=10;
const int Penalty_Factor_Overdue=10;
int Time_Limit_Per_Order=30;
const int Profit_Per_Order=10;
const int Penalty_Per_Order=50;

extern int CurrentTime;
extern Order* OrderListHead;
extern int OrderCnt;
extern int RiderCnt;
extern Rider* RiderListHead;
extern int Balance;
extern Rider* LastRider;
extern int OrderOverdue;
extern int OrderCompleted;
extern int OrderTaken;
extern Queue OverdueQueue;
extern Queue CompletedQueue;

int matDis(Anchor *a,Anchor *b);
//计算两点的曼哈顿距离
Anchor* duplicateAnchor(Anchor *ori);
//复制一个锚点到一个新锚点
Anchor* duplicateAnchorList(Anchor *ori);
//复制一个链表到新链表
Anchor* appendOrderAssumption(Rider *rider,Anchor *src_ori,Anchor *dst_ori);
//根据骑手当前的位置，将src和dst插入后传回假设的路径
int findMinIndex(int arr[],int n);
//找出数组的最小值
Rider *hireNewbie();
//创建新人骑手节点
void assginSingleOrder(Order *o);
//分配单个订单
void freeAnchorList(Anchor *LinkedListHead);
//释放带头结点的链表，头结点也释放
void updateSingleRider(Rider *rider);
//更新单个骑手的状态
void settleOrder(Order *o);
//结算单笔订单
int isEnd();
//判断派单过程是否结束
void moveRider(Rider *rider);
//根据pos和路径，移动骑手到下一个位置（更新pos）
void updateAllRiders();
//更新所有骑手
void assignOrderPeriodic(int CurrentTime);
//分配某时间刻的所有订单
int estimate(Anchor *path,Rider *rider);
//评估生成路径的优劣
void OrderOverdueFunc(Order *o);
//订单结算时超时的处理
void OrderSuccessFunc(Order *o);
//订单结算时送达的处理

Order* OrderClipStrategy=NULL;

int matDis(Anchor *a,Anchor *b)
{
	if(!a||!b) return 0;
	return abs(a->x-b->x)+abs(a->y-b->y);//否则计算曼哈顿距离
}

Anchor *duplicateAnchor(Anchor *ori)
{
	if(!ori) return NULL;
	Anchor *currentPtr=(Anchor*)malloc(sizeof(Anchor));
	currentPtr->x=ori->x;
	currentPtr->y=ori->y;
	currentPtr->from=ori->from;
	currentPtr->nextPtr=ori->nextPtr;
	return currentPtr;
}

Anchor* duplicateAnchorList(Anchor *ori)
{
	Anchor *r=(Anchor*)malloc(sizeof(Anchor));//新链表的头哨兵
	Anchor *iterator=ori->nextPtr;//原链表的迭代器
	Anchor *currentPtr=NULL,*lastPtr=NULL;
	while(iterator)//迭代原链表
	{
		currentPtr=duplicateAnchor(iterator);//复制节点的数据到新节点
		if(!lastPtr)//新链表为空时
		{
			r->nextPtr=currentPtr;
			lastPtr=currentPtr;
		}
		else
		{
			lastPtr->nextPtr=currentPtr;
			lastPtr=currentPtr;
		}
		iterator=iterator->nextPtr;
	}
	if(!lastPtr) r->nextPtr=NULL;//原链表为空时，新链表也为空
	else lastPtr->nextPtr=NULL;//否则给新链表封尾
	return r;
}

Anchor* appendOrderAssumption(Rider *rider,Anchor *src_ori,Anchor *dst_ori)
{
	Anchor *src=duplicateAnchor(src_ori),*dst=duplicateAnchor(dst_ori);
	src->nextPtr=NULL;
	dst->nextPtr=NULL;

	Anchor *tmpPath=duplicateAnchorList(rider->path);
	//复制一份骑手的路径
	Anchor *tmpPos=duplicateAnchor(rider->pos);
	//复制一份骑手的位置
	tmpPos->nextPtr=tmpPath->nextPtr;
	tmpPath->nextPtr=tmpPos;
	//把骑手位置加到链表后面去

	Anchor *iterator=tmpPath->nextPtr;
	int dis[rider->nodeCnt+1];
	int i=0;
	while(iterator)
	{
		dis[i]=matDis(iterator,src)+matDis(iterator->nextPtr,src);
		//统计如果插入该位置，需要增加的路程
		if(iterator->nextPtr) dis[i]+=CurrentTime-(iterator->nextPtr->from->time);
		++i;
		//考虑当前执行订单剩余时间
		iterator=iterator->nextPtr;
	}
	
	int chosen_loc=findMinIndex(dis,i);

	iterator=tmpPath->nextPtr;//迭代器归位
	while(chosen_loc--)//迭代chosen_loc次
		iterator=iterator->nextPtr;
	
	src->nextPtr=iterator->nextPtr;
	iterator->nextPtr=src;
	//插入了src

	iterator=src;//找dst插入的位置时，迭代器放在src
	i=0;
	while(iterator)
	{
		dis[i]=matDis(iterator,dst)+matDis(iterator->nextPtr,dst);
		if(iterator->nextPtr) dis[i]+=CurrentTime-(iterator->nextPtr->from->time);
		++i;
		iterator=iterator->nextPtr;
	}
	chosen_loc=findMinIndex(dis,i);

	iterator=src;
	while(chosen_loc--)
		iterator=iterator->nextPtr;

	dst->nextPtr=iterator->nextPtr;
	iterator->nextPtr=dst;
	//插入了dst
	tmpPath->nextPtr=tmpPos->nextPtr;
	free(tmpPos);
	//删除临时骑手节点，得到完整的新路径
	//printPath(tmpPath);
	return tmpPath;
}

int findMinIndex(int arr[],int n)//返回的是拥有最小值的最后的元素下标
{
	if(!n) return -1;
	int i,min=arr[0],loc=0;
	for(i=0;i<n;++i)
		if(arr[i]<=min)
		{
			min=arr[i];
			loc=i;
		}
	return loc;
}

Rider *hireNewbie()
{
	//创建假想新人NewBie
	Rider *Newbie=(Rider*)malloc(sizeof(Rider));
	Newbie->nodeCnt=0;

	Newbie->pos=(Anchor*)malloc(sizeof(Anchor));
	//现在新骑手的坐标锚点需要自己申请
	Newbie->pos->x=Default_Born_Point_X;
	Newbie->pos->y=Default_Born_Point_Y;

	Newbie->path=(Anchor*)malloc(sizeof(Anchor));
	Newbie->path->nextPtr=NULL;
	//新人的路径链表为空

	Newbie->stopbyNodes=(Anchor*)malloc(sizeof(Anchor));
	Newbie->stopbyNodes->nextPtr=NULL;
	Newbie->lastStopby=NULL;
	//新人的停靠列表为空

	Newbie->nextPtr=NULL;
	//新人在骑手链表中后面为空
	return Newbie;
}

void printPath(Anchor *path)
{
	FILE *fPtr=fopen("debug.txt","a+");
	Anchor* iterator=path->nextPtr;
	while(iterator)
	{
		fprintf(fPtr,"(%d,%d)->",iterator->x,iterator->y );
		iterator=iterator->nextPtr;
	}
	fputc('\n',fPtr);
	fflush(fPtr);
	fclose(fPtr);
}

void assginSingleOrder(Order *o)
{
	Anchor *NewPaths[RiderCnt+1];
	Rider *RiderAddress[RiderCnt+1];
	int scoreboard[RiderCnt+1];
	int i=1;

	//评估已有骑手
	Rider *iterator=RiderListHead->nextPtr;
	while(iterator)
	{
		RiderAddress[i]=iterator;
		NewPaths[i]=appendOrderAssumption(iterator,o->src,o->dst);
		scoreboard[i]=estimate(NewPaths[i],iterator)+iterator->nodeCnt;
		i++;
		iterator=iterator->nextPtr;
		//遍历骑手，分别假设生成新路径存入NewPaths，计分
	}
	//判断是否能招新

	Rider *Newbie;
	if(Balance>=300)//有钱才考虑招新
	{
		Newbie=hireNewbie();
		NewPaths[0]=appendOrderAssumption(Newbie,o->src,o->dst);
		scoreboard[0]=estimate(NewPaths[0],Newbie);
		scoreboard[0]+=Penalty_Factor_Newbie+Newbie->nodeCnt;
	}
	else
		scoreboard[0]=INF;//不然新人的评估值为无穷大，肯定不会选到
	//找出和判断

	int chosen_rider=findMinIndex(scoreboard,RiderCnt+1);
	if(!chosen_rider)//如果选到了新人
	{
		for(i=1;i<=RiderCnt;++i)
			freeAnchorList(NewPaths[i]);
		//释放其他人的假想路径
		Newbie->path=NewPaths[0];

		++RiderCnt;
		Newbie->nodeCnt+=2;//需要经过的点数加2

		if(!LastRider)
			RiderListHead->nextPtr=Newbie;
		else
			LastRider->nextPtr=Newbie;
		LastRider=Newbie;
		//让新人加入队伍
		Balance-=300;
	}
	else
	{
		for(i=0;i<=RiderCnt;++i)
		{	
			if(i==0&&scoreboard[0]==INF)//说明没有招新
				continue;//不处理，因为没有路径
			if(i==chosen_rider)
				continue;
			freeAnchorList(NewPaths[i]);
		}

		freeAnchorList(RiderAddress[chosen_rider]->path);
		RiderAddress[chosen_rider]->path=NewPaths[chosen_rider];
		RiderAddress[chosen_rider]->nodeCnt+=2;//增加两个路径点

		if(scoreboard[0]!=INF)//说明招新了但没被选中
		{
			free(Newbie->pos); 
			free(Newbie->path);
			free(Newbie);
		}
	}
	++OrderTaken;

	//订单被派出
	o->state=1;
}

void freeAnchorList(Anchor *LinkedListHead)
{
	Anchor *tmpPtr;
	while(LinkedListHead)
	{
		tmpPtr=LinkedListHead;
		LinkedListHead=LinkedListHead->nextPtr;
		free(tmpPtr);
	}	
}

void updateSingleRider(Rider *rider)
{
	moveRider(rider);
	Anchor *iterator=rider->path->nextPtr;
	Anchor *currentPtr;
	//清空停靠状态
	freeAnchorList(rider->stopbyNodes->nextPtr);
	rider->stopbyNodes->nextPtr=NULL;
	rider->lastStopby=NULL;

	while(matDis(rider->pos,iterator)==1)
	{
		currentPtr=duplicateAnchor(iterator);
		currentPtr->nextPtr=NULL;
		if(!(rider->lastStopby))
			rider->stopbyNodes->nextPtr=currentPtr;
		else
			rider->lastStopby->nextPtr=currentPtr;
		rider->lastStopby=currentPtr;

		if(iterator->from->dst->x==iterator->from->src->x && iterator->from->dst->y==iterator->from->src->y)
		{
			if(!iterator->nextPtr)
			{
				settleOrder(iterator->from);
				rider->nodeCnt-=2;
			}
			else if(iterator->nextPtr->from->id!=iterator->from->id)
			{
				settleOrder(iterator->from);
				rider->nodeCnt-=2;
			}
		}
		else if(iterator->from->dst->x==iterator->x && iterator->from->dst->y==iterator->y)//说明是dst
			{
				settleOrder(iterator->from);
				rider->nodeCnt-=2;
			}
		//dst就多一步结算
		iterator=iterator->nextPtr;
	}
	rider->path->nextPtr=iterator;
}

void settleOrder(Order *o)
{
	int time_differ=CurrentTime-(o->time);
	if(time_differ>Time_Limit_Per_Order)
		OrderOverdueFunc(o);
	else
		OrderSuccessFunc(o);

	o->state=2;	
}
extern int input_flag;
int isEnd()
{
	if(Balance<0) return 1;
	if(input_flag == 0 && OrderCompleted==OrderCnt) return 1;
	return 0;
}

void moveRider(Rider *rider)//较为圆滑的走位
{
	int X_flag = 2, Y_flag = 2, flagx = 0, flagy = 0;//判断下一位置和下下位置的关系，如在上方则y=0,在左方则x=0
	if (rider->path->nextPtr)
	{
		if (rider->path->nextPtr->nextPtr)
		{
			if ((rider->path->nextPtr->nextPtr->x) > (rider->path->nextPtr->x))
				X_flag = 1;
			if ((rider->path->nextPtr->nextPtr->y) > (rider->path->nextPtr->y))
				Y_flag = 1;
			if ((rider->path->nextPtr->nextPtr->x) < (rider->path->nextPtr->x))
				X_flag = 0;
			if ((rider->path->nextPtr->nextPtr->y) < (rider->path->nextPtr->y))
				Y_flag = 0;
			if(abs(rider->pos->x - rider->path->nextPtr->nextPtr->x)==1)
				flagx=1;
			if(abs(rider->pos->y - rider->path->nextPtr->nextPtr->y)==1)
				flagy=1;
		}
		if ((rider->pos->x) % 2)//如果骑手在竖着的路上
		{
			if ((rider->pos->y) - (rider->path->nextPtr->y) > 2 || ((rider->pos->y) - (rider->path->nextPtr->y) == 2 && Y_flag == 0))
				rider->pos->y = rider->pos->y - 2;
			else if ((rider->pos->y) - (rider->path->nextPtr->y) < -2 || ((rider->pos->y) - (rider->path->nextPtr->y) == -2 && (Y_flag == 1)))
				rider->pos->y = rider->pos->y + 2;
			else
			{
				if ((rider->pos->y) - (rider->path->nextPtr->y) == 2 || ((rider->pos->y) - (rider->path->nextPtr->y) == 0)&&Y_flag==0 || rider->pos->y==16)
					rider->pos->y = rider->pos->y - 1;
				else
					rider->pos->y = rider->pos->y + 1;
				if ((rider->pos->x) > (rider->path->nextPtr->x))
					rider->pos->x = rider->pos->x - 1;
				else
					rider->pos->x = rider->pos->x + 1;
			}
		}
		else//骑手横着的路上
		{
			if ((rider->pos->x) - (rider->path->nextPtr->x) > 2 || ((rider->pos->x) - (rider->path->nextPtr->x) == 2 && (X_flag == 0)))
				rider->pos->x = rider->pos->x - 2;
			else if ((rider->pos->x) - (rider->path->nextPtr->x) < -2 || ((rider->pos->x) - (rider->path->nextPtr->x) == -2 && (X_flag == 1)))
				rider->pos->x = rider->pos->x + 2;
			else
			{
				if ((rider->pos->x) - (rider->path->nextPtr->x) == 2 || ((rider->pos->x) - (rider->path->nextPtr->x) == 0)&&X_flag==0 || rider->pos->x==16)
					rider->pos->x = rider->pos->x - 1;
				else
					rider->pos->x = rider->pos->x + 1;
				if ((rider->pos->y) > (rider->path->nextPtr->y))
					rider->pos->y = rider->pos->y - 1;
				else
					rider->pos->y = rider->pos->y + 1;
			}
		}
	}
}

void updateAllRiders()
{
	Rider *iterator=RiderListHead->nextPtr;
	while(iterator)
	{
		updateSingleRider(iterator);
		iterator=iterator->nextPtr;
	}
}

void assignOrderPeriodic(int CurrentTime)
{
	Order *iterator=OrderListHead->nextPtr;
	while(iterator)
	{
		if(!(iterator->state) && (iterator->time)<=CurrentTime)
			assginSingleOrder(iterator);
		iterator=iterator->nextPtr;
	}
}

int estimate(Anchor *path,Rider *rider)
{
	Anchor *iterator=path->nextPtr;
	Anchor *lastPos=rider->pos;
	int score=0,distance=0,time_differ;
	while(iterator)
	{
		distance+=matDis(lastPos,iterator);
		if((iterator->from->dst->x==iterator->x && iterator->from->dst->y==iterator->y))//读取到了终点
		{
			time_differ= (CurrentTime + distance/velocity) - (iterator->from->time);
			if(time_differ>Time_Limit_Per_Order)
				score+=Penalty_Factor_Overdue;
		}
		lastPos=iterator;
		iterator=iterator->nextPtr;
	}
	return score;
}

extern void addToQueue(Queue q,int num);
void OrderOverdueFunc(Order *o)
{
	OrderOverdue++;
	OrderCompleted++;
	Balance-=Penalty_Per_Order;
	addToQueue(OverdueQueue,o->id);
}

void OrderSuccessFunc(Order *o)
{
	OrderCompleted++;
	Balance+=Profit_Per_Order;
	addToQueue(CompletedQueue,o->id);
}

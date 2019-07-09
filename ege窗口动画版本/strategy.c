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
//��������������پ���
Anchor* duplicateAnchor(Anchor *ori);
//����һ��ê�㵽һ����ê��
Anchor* duplicateAnchorList(Anchor *ori);
//����һ������������
Anchor* appendOrderAssumption(Rider *rider,Anchor *src_ori,Anchor *dst_ori);
//�������ֵ�ǰ��λ�ã���src��dst����󴫻ؼ����·��
int findMinIndex(int arr[],int n);
//�ҳ��������Сֵ
Rider *hireNewbie();
//�����������ֽڵ�
void assginSingleOrder(Order *o);
//���䵥������
void freeAnchorList(Anchor *LinkedListHead);
//�ͷŴ�ͷ��������ͷ���Ҳ�ͷ�
void updateSingleRider(Rider *rider);
//���µ������ֵ�״̬
void settleOrder(Order *o);
//���㵥�ʶ���
int isEnd();
//�ж��ɵ������Ƿ����
void moveRider(Rider *rider);
//����pos��·�����ƶ����ֵ���һ��λ�ã�����pos��
void updateAllRiders();
//������������
void assignOrderPeriodic(int CurrentTime);
//����ĳʱ��̵����ж���
int estimate(Anchor *path,Rider *rider);
//��������·��������
void OrderOverdueFunc(Order *o);
//��������ʱ��ʱ�Ĵ���
void OrderSuccessFunc(Order *o);
//��������ʱ�ʹ�Ĵ���

Order* OrderClipStrategy=NULL;

int matDis(Anchor *a,Anchor *b)
{
	if(!a||!b) return 0;
	return abs(a->x-b->x)+abs(a->y-b->y);//������������پ���
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
	Anchor *r=(Anchor*)malloc(sizeof(Anchor));//�������ͷ�ڱ�
	Anchor *iterator=ori->nextPtr;//ԭ����ĵ�����
	Anchor *currentPtr=NULL,*lastPtr=NULL;
	while(iterator)//����ԭ����
	{
		currentPtr=duplicateAnchor(iterator);//���ƽڵ�����ݵ��½ڵ�
		if(!lastPtr)//������Ϊ��ʱ
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
	if(!lastPtr) r->nextPtr=NULL;//ԭ����Ϊ��ʱ��������ҲΪ��
	else lastPtr->nextPtr=NULL;//������������β
	return r;
}

Anchor* appendOrderAssumption(Rider *rider,Anchor *src_ori,Anchor *dst_ori)
{
	Anchor *src=duplicateAnchor(src_ori),*dst=duplicateAnchor(dst_ori);
	src->nextPtr=NULL;
	dst->nextPtr=NULL;

	Anchor *tmpPath=duplicateAnchorList(rider->path);
	//����һ�����ֵ�·��
	Anchor *tmpPos=duplicateAnchor(rider->pos);
	//����һ�����ֵ�λ��
	tmpPos->nextPtr=tmpPath->nextPtr;
	tmpPath->nextPtr=tmpPos;
	//������λ�üӵ��������ȥ

	Anchor *iterator=tmpPath->nextPtr;
	int dis[rider->nodeCnt+1];
	int i=0;
	while(iterator)
	{
		dis[i]=matDis(iterator,src)+matDis(iterator->nextPtr,src);
		//ͳ����������λ�ã���Ҫ���ӵ�·��
		if(iterator->nextPtr) dis[i]+=CurrentTime-(iterator->nextPtr->from->time);
		++i;
		//���ǵ�ǰִ�ж���ʣ��ʱ��
		iterator=iterator->nextPtr;
	}
	
	int chosen_loc=findMinIndex(dis,i);

	iterator=tmpPath->nextPtr;//��������λ
	while(chosen_loc--)//����chosen_loc��
		iterator=iterator->nextPtr;
	
	src->nextPtr=iterator->nextPtr;
	iterator->nextPtr=src;
	//������src

	iterator=src;//��dst�����λ��ʱ������������src
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
	//������dst
	tmpPath->nextPtr=tmpPos->nextPtr;
	free(tmpPos);
	//ɾ����ʱ���ֽڵ㣬�õ���������·��
	//printPath(tmpPath);
	return tmpPath;
}

int findMinIndex(int arr[],int n)//���ص���ӵ����Сֵ������Ԫ���±�
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
	//������������NewBie
	Rider *Newbie=(Rider*)malloc(sizeof(Rider));
	Newbie->nodeCnt=0;

	Newbie->pos=(Anchor*)malloc(sizeof(Anchor));
	//���������ֵ�����ê����Ҫ�Լ�����
	Newbie->pos->x=Default_Born_Point_X;
	Newbie->pos->y=Default_Born_Point_Y;

	Newbie->path=(Anchor*)malloc(sizeof(Anchor));
	Newbie->path->nextPtr=NULL;
	//���˵�·������Ϊ��

	Newbie->stopbyNodes=(Anchor*)malloc(sizeof(Anchor));
	Newbie->stopbyNodes->nextPtr=NULL;
	Newbie->lastStopby=NULL;
	//���˵�ͣ���б�Ϊ��

	Newbie->nextPtr=NULL;
	//���������������к���Ϊ��
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

	//������������
	Rider *iterator=RiderListHead->nextPtr;
	while(iterator)
	{
		RiderAddress[i]=iterator;
		NewPaths[i]=appendOrderAssumption(iterator,o->src,o->dst);
		scoreboard[i]=estimate(NewPaths[i],iterator)+iterator->nodeCnt;
		i++;
		iterator=iterator->nextPtr;
		//�������֣��ֱ����������·������NewPaths���Ʒ�
	}
	//�ж��Ƿ�������

	Rider *Newbie;
	if(Balance>=300)//��Ǯ�ſ�������
	{
		Newbie=hireNewbie();
		NewPaths[0]=appendOrderAssumption(Newbie,o->src,o->dst);
		scoreboard[0]=estimate(NewPaths[0],Newbie);
		scoreboard[0]+=Penalty_Factor_Newbie+Newbie->nodeCnt;
	}
	else
		scoreboard[0]=INF;//��Ȼ���˵�����ֵΪ����󣬿϶�����ѡ��
	//�ҳ����ж�

	int chosen_rider=findMinIndex(scoreboard,RiderCnt+1);
	if(!chosen_rider)//���ѡ��������
	{
		for(i=1;i<=RiderCnt;++i)
			freeAnchorList(NewPaths[i]);
		//�ͷ������˵ļ���·��
		Newbie->path=NewPaths[0];

		++RiderCnt;
		Newbie->nodeCnt+=2;//��Ҫ�����ĵ�����2

		if(!LastRider)
			RiderListHead->nextPtr=Newbie;
		else
			LastRider->nextPtr=Newbie;
		LastRider=Newbie;
		//�����˼������
		Balance-=300;
	}
	else
	{
		for(i=0;i<=RiderCnt;++i)
		{	
			if(i==0&&scoreboard[0]==INF)//˵��û������
				continue;//��������Ϊû��·��
			if(i==chosen_rider)
				continue;
			freeAnchorList(NewPaths[i]);
		}

		freeAnchorList(RiderAddress[chosen_rider]->path);
		RiderAddress[chosen_rider]->path=NewPaths[chosen_rider];
		RiderAddress[chosen_rider]->nodeCnt+=2;//��������·����

		if(scoreboard[0]!=INF)//˵�������˵�û��ѡ��
		{
			free(Newbie->pos); 
			free(Newbie->path);
			free(Newbie);
		}
	}
	++OrderTaken;

	//�������ɳ�
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
	//���ͣ��״̬
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
		else if(iterator->from->dst->x==iterator->x && iterator->from->dst->y==iterator->y)//˵����dst
			{
				settleOrder(iterator->from);
				rider->nodeCnt-=2;
			}
		//dst�Ͷ�һ������
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

void moveRider(Rider *rider)//��ΪԲ������λ
{
	int X_flag = 2, Y_flag = 2, flagx = 0, flagy = 0;//�ж���һλ�ú�����λ�õĹ�ϵ�������Ϸ���y=0,������x=0
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
		if ((rider->pos->x) % 2)//������������ŵ�·��
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
		else//���ֺ��ŵ�·��
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
		if((iterator->from->dst->x==iterator->x && iterator->from->dst->y==iterator->y))//��ȡ�����յ�
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

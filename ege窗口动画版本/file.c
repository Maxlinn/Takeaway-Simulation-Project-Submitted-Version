#include "define.h"
#include <stdlib.h>
#include <stdio.h>

extern Order* OrderListHead;
extern int OrderCnt;
extern int RiderCnt;
extern int CurrentTime;
extern int Balance;
extern int OrderTaken;
extern int OrderCompleted;
extern int OrderOverdue;
extern Rider* RiderListHead;
extern Queue OverdueQueue;
extern Queue CompletedQueue;
enum STOPBYSTATUS{
	NOSTOPPING,RESTRAURANT,CUSTOMER,RESTCUST
};//停靠的状态，即没有停靠，停靠点是餐馆，停靠点是食客
Node *OrderOverdueClipFile=NULL;//记录超时的订单链表打印的位置
Node *OrderCompletedClipFile=NULL;//记录完成的订单链表打印的位置

int commandWindowInput();//命令行输入
int fileInput(FILE *fPtr);//文件输入
int isFileAvailable();//判断文件是否可读
int salesInput();//总的读入函数
int isFileEmpty();//判断文件是否为空
void updateFile();//更新输出文件
void sortOrder(Order *OrderListHead,int n);//将输入的订单按时间递增排序
void debug_printOrders();//打印订单
void debug_printRiders();//打印骑手的所有信息

int isQueueEmpty(Queue q);
Queue createEmptyQueue();
void addToQueue(Queue q,int num);
int outFromQueue(Queue q);

void fileInput()
{
	FILE *fPtr=fopen("sales.txt","r");
	
	int id,time,src_x,src_y,dst_x,dst_y;
	Order *lastPtr=OrderListHead->nextPtr,*currentPtr=NULL;
	while((fscanf(fPtr,"%d%d%d%d%d%d",&id,&time,&src_x,&src_y,&dst_x,&dst_y))!=EOF)
	{
		currentPtr=(Order*)malloc(sizeof(Order));
		currentPtr->id=id;
		currentPtr->time=time;
		currentPtr->state=0;

		currentPtr->src=(Anchor*)malloc(sizeof(Anchor));
		currentPtr->dst=(Anchor*)malloc(sizeof(Anchor));

		currentPtr->src->x=src_x;
		currentPtr->src->y=src_y;
		currentPtr->src->from=currentPtr;
		currentPtr->src->nextPtr=NULL;
		//填写订单中的src锚点，src可以通过from指针找出自己来自的订单
		currentPtr->dst->x=dst_x;
		currentPtr->dst->y=dst_y;
		currentPtr->dst->from=currentPtr;
		currentPtr->dst->nextPtr=NULL;
		//填写dst锚点
		if(!lastPtr) 
		{
			OrderListHead->nextPtr=currentPtr;
			lastPtr=currentPtr;
		}
		else
		{
			lastPtr->nextPtr=currentPtr;
			lastPtr=currentPtr;
		}
		//生成order的链表
		++OrderCnt;//订单计数
	}
	lastPtr->nextPtr=NULL;
	fclose(fPtr);
	//给订单链表封尾
}

int isFileAvailable()
{	
	FILE *fPtr=fopen("sales.txt","r");
	int flag=(fPtr)?1:0;
	fclose(fPtr);
	return flag;
}


void updateFile()
{
	FILE *fPtr=fopen("output.txt","a+");
	fprintf(fPtr, "时间：%d\n",CurrentTime );
	fprintf(fPtr, "钱：%d\n",Balance );
	fprintf(fPtr, "接单数：%d\n",OrderTaken );

	fprintf(fPtr, "完成数：%d；结单：",OrderCompleted );
	while(!isQueueEmpty(CompletedQueue))
		fprintf(fPtr, "%d ",outFromQueue(CompletedQueue) );
	fputc('\n',fPtr);

	fprintf(fPtr, "超时数：%d；罚单：",OrderOverdue);
	while(!isQueueEmpty(OverdueQueue))
		fprintf(fPtr, "%d ",outFromQueue(OverdueQueue) );
	fputc('\n',fPtr);

	Rider *iterator=RiderListHead->nextPtr;
	Anchor *iterator_a,*prevPtr_1=NULL,*prevPtr_2=NULL;//prevPtr一和二分别储存可能停靠的不同的两个点
	int i=0,j=0,state_1=NOSTOPPING,state_2=NOSTOPPING;//state一和二分别表示所停靠两个点的状态
	while(iterator)
	{
		iterator_a=iterator->stopbyNodes->nextPtr;
		while(iterator_a)
		{
			if(!prevPtr_1)//出现第一个停靠点
			{	
				if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y) state_1=RESTRAURANT;
				else state_1=CUSTOMER;
				prevPtr_1=iterator_a;
			}
			else 
			{
				if(!prevPtr_2 && (iterator_a->x!=prevPtr_1->x||iterator_a->y!=prevPtr_1->y))//出现第二个停靠点且坐标与第一点不同
				{
					prevPtr_2=iterator_a;
					if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y) state_2=RESTRAURANT;
					else state_2=CUSTOMER;
				}
				else if(!prevPtr_2 || iterator_a->x==prevPtr_1->x && iterator_a->y==prevPtr_1->y)//停靠点与第一点相同时
				{
					if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y && state_1==CUSTOMER) state_1=RESTCUST;
					if(iterator_a->x==iterator_a->from->dst->x && iterator_a->y==iterator_a->from->dst->y && state_1==RESTRAURANT) state_1=RESTCUST;
				}
				else//停靠点与第二点相同时
				{
					if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y && state_2==CUSTOMER) state_2=RESTCUST;
					if(iterator_a->x==iterator_a->from->dst->x && iterator_a->y==iterator_a->from->dst->y && state_2==RESTRAURANT) state_2=RESTCUST;
				}
			}
			iterator_a=iterator_a->nextPtr; 
		}
		fprintf(fPtr, "骑手%d位置：%d，%d;停靠：",i++,iterator->pos->x,iterator->pos->y);
		switch(state_1)
		{
			case NOSTOPPING:break;
			case RESTRAURANT:fprintf(fPtr, "餐馆 %d %d ",prevPtr_1->x,prevPtr_1->y);break;
			case CUSTOMER:fprintf(fPtr, "食客 %d %d ",prevPtr_1->x,prevPtr_1->y);break;
			case RESTCUST:fprintf(fPtr, "餐客 %d %d ",prevPtr_1->x,prevPtr_1->y);break;
		}
		switch(state_2)
		{
			case NOSTOPPING:break;
			case RESTRAURANT:fprintf(fPtr, "餐馆 %d %d ",prevPtr_2->x,prevPtr_2->y);break;
			case CUSTOMER:fprintf(fPtr, "食客 %d %d ",prevPtr_2->x,prevPtr_2->y);break;
			case RESTCUST:fprintf(fPtr, "餐客 %d %d ",prevPtr_2->x,prevPtr_2->y);break;
		}
		fputc(';',fPtr);
		fputc('\n',fPtr);

		iterator=iterator->nextPtr;
	}
	fputc('\n',fPtr);
	fflush(fPtr);
	fclose(fPtr);
}

void sortOrder(Order *OrderListHead,int n)
{
	int i,j,isChanged;
	//需要设置isChanged标志位，交换与否对之后的节点迭代有影响，要分开考虑
	if(n==0||n==1) return ;//空链表或者只有一个元素有什么好排序的
	Order *aPtr,*bPtr,*prevPtr;
	for(i=1;i<n;++i)//冒泡排序，共跑n-1趟，仅作为计数，没有啥意义
	{
		prevPtr=NULL;//每跑一趟，初始时前置节点设为NULL
		aPtr=OrderListHead->nextPtr;//a是链表头结点，因为在排序中可能会改变链表的入口，每次需要重新获取
		bPtr=aPtr->nextPtr;//b是第二个节点，冒泡排序是相邻两个排序嘛
		isChanged=0;//标志位，交换与不交换时，迭代的操作时不一样的
		for(j=0;j<n-i;++j)//冒泡每趟检查n-i-1次
		{
			if(!prevPtr)//没有前置节点，即为头结点时
			{
				if((aPtr->time)>(bPtr->time))//说明需要交换头结点和第二个节点
				{
					OrderListHead->nextPtr=bPtr;//链表的入口改成b节点
					aPtr->nextPtr=bPtr->nextPtr;//a节点的后继变成b节点的后继
					bPtr->nextPtr=aPtr;//把a节点接到b后面
					isChanged=1;//标志位，说明发生了改变
				}
				if(isChanged)//若交换过前两个节点
				{
					bPtr=aPtr->nextPtr;//还是保持a前b后
					isChanged=0;
				}
				else//没有交换就各自向后迭代
				{
					bPtr=bPtr->nextPtr;
					aPtr=aPtr->nextPtr;
				}
				prevPtr=OrderListHead->nextPtr;//prevPtr指向链表入口
			}
			else//不是头结点，即对中间的两个节点检查时
			{
				if((aPtr->time)>(bPtr->time))
				{
					isChanged=1;
					aPtr->nextPtr=bPtr->nextPtr;//a的后继改成b的后继
					prevPtr->nextPtr=bPtr;//把b接上前面节点
					bPtr->nextPtr=aPtr;//a节点接到b节点后面
				}
				if(isChanged)//交换过
				{
					isChanged=0;
					prevPtr=prevPtr->nextPtr;//前置节点向后迭代
					bPtr=aPtr->nextPtr;//保持a前b后
					aPtr=prevPtr->nextPtr;
				}
				else//三个指针各自向后迭代
				{
					bPtr=bPtr->nextPtr;
					aPtr=aPtr->nextPtr;
					prevPtr=prevPtr->nextPtr;
				}
			}
		}
	}
}

void debug_printOrders()
{
	Order *iterator=OrderListHead->nextPtr;
	FILE *f=fopen("ORDERS.txt","a+"); 
	while(iterator)
	{
		fprintf(f,"Orderid:%d OrderTime:%d src:%d %d dst:%d %d\n",iterator->id,iterator->time,iterator->src->x,iterator->src->y,iterator->dst->x,iterator->dst->y );
		iterator=iterator->nextPtr;
	}
	fflush(f);
}


void debug_printRiders()
{
	FILE *fPtr=fopen("RIDERS.txt","a+");
	fprintf(fPtr, "钱：%d，当前时间：%d\n骑手数量：%d\n", Balance,CurrentTime,RiderCnt);
	Rider *iterator=RiderListHead->nextPtr;
	Anchor *way_iterator;
	int i=0;
	while(iterator)
	{
		fprintf(fPtr, "骑手：%d\n",i++ );
		fprintf(fPtr, "NodeCnt:%d\n",iterator->nodeCnt );
		fprintf(fPtr, "Pos:%d %d\n",iterator->pos->x,iterator->pos->y );
		way_iterator=iterator->path->nextPtr;
		fprintf(fPtr, "路径：\n");
		while(way_iterator)
		{
			fprintf(fPtr, "(%d,%d)->", way_iterator->x,way_iterator->y);
			way_iterator=way_iterator->nextPtr;
		}
		fprintf(fPtr, "\n\n");
		iterator=iterator->nextPtr;
	}
	fflush(fPtr);
	fclose(fPtr);
}

Queue createEmptyQueue()
{
	Queue q=(Queue)malloc(sizeof(QNode));
	q->front=NULL;
	q->rear=NULL;
	return q;
}

void addToQueue(Queue q,int num)
{
	Node *currentNode=(Node*)malloc(sizeof(Node));
	currentNode->num=num;
	currentNode->nextPtr=NULL;

	if(!(q->front)&&!(q->rear))
	{
		q->front=currentNode;
		q->rear=currentNode;
	}
	else
	{
		q->rear->nextPtr=currentNode;
		q->rear=currentNode;
	}
}

int outFromQueue(Queue q)
{
	//没有做空队列检测，请在out之前自行检测
	Node *frontCell=q->front;
	if(q->front==q->rear)//队列只有一个元素
		q->front=q->rear=NULL;//删除后队列置空
	else
		q->front=q->front->nextPtr;
	int r=frontCell->num;
	free(frontCell);
	return r;
}

int isQueueEmpty(Queue q)
{
	if(!q->front)
		return 1;
	return 0;
}

int isFileEmpty()
{
	FILE* f=fopen("sales.txt","r");
	fgetc(f);
	int flag=(feof(f))?1:0;
	fclose(f);
	return flag;
}

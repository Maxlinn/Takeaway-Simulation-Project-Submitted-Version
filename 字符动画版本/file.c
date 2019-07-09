#include "define.h"
#include <stdlib.h>
#include <stdio.h>

extern Order* OrderListHead;
extern int RiderCnt;
extern int CurrentTime;
extern int Balance;
extern int OrderTaken;
extern int OrderCompleted;
extern int OrderOverdue;
extern Rider* RiderListHead;
extern Node* LastOrderCompleted;
extern Node* OrderCompletedList;
extern Node* LastOrderOverdue;
extern Node* OrderOverdueList;
enum STOPBYSTATUS{
	NOSTOPPING,RESTRAURANT,CUSTOMER
};

int commandWindowInput();
int fileInput(FILE *fPtr);
int isFileAvailable();
int salesInput();
void updateFile();
void sortOrder(Order *OrderListHead,int n);
void debug_printOrders();
void debug_printAllThings();

int commandWindowInput()
{
	int id,time,src_x,src_y,dst_x,dst_y;
	int cnt=0;
	Order *lastPtr=OrderListHead->nextPtr,*currentPtr=NULL;
	while((scanf("%d%d%d%d%d%d",&id,&time,&src_x,&src_y,&dst_x,&dst_y))!=EOF)
	{
		currentPtr=(Order*)malloc(sizeof(Order));
		currentPtr->id=id;
		currentPtr->time=time;

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
		++cnt;//订单计数
	}
	lastPtr->nextPtr=NULL;
	//给订单链表封尾
	return cnt;
}

int fileInput(FILE *fPtr)
{
	int id,time,src_x,src_y,dst_x,dst_y;
	int cnt=0;
	Order *lastPtr=OrderListHead->nextPtr,*currentPtr=NULL;
	while((fscanf(fPtr,"%d%d%d%d%d%d",&id,&time,&src_x,&src_y,&dst_x,&dst_y))!=EOF)
	{
		currentPtr=(Order*)malloc(sizeof(Order));
		currentPtr->id=id;
		currentPtr->time=time;

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
		++cnt;//订单计数
	}
	lastPtr->nextPtr=NULL;
	//给订单链表封尾
	return cnt;
}

int isFileAvailable()
{
	FILE *fPtr=fopen("sales.txt","r");
	int flag=(fPtr)?1:0;
	fclose(fPtr);
	return flag;
}

int salesInput()
{
	FILE *fPtr=fopen("sales.txt","r");
	int numOfSales=0;
	if(fPtr)//激活文件输入
	{
		numOfSales=fileInput(fPtr);
		fclose(fPtr);
	}
	else
	{
		printf("Please input sales:\n");
		numOfSales=commandWindowInput();
	}
	return numOfSales;
}

void updateFile()
{
	FILE *fPtr=fopen("output.txt","a+");
	fprintf(fPtr, "时间：%d\n",CurrentTime );
	fprintf(fPtr, "钱：%d\n",Balance );
	fprintf(fPtr, "接单数：%d\n",OrderTaken );

	fprintf(fPtr, "完成数：%d；结单：",OrderCompleted );
	Node *iterator_Node=OrderCompletedList->nextPtr;
	while(iterator_Node)
	{
		fprintf(fPtr, "%d ",iterator_Node->num );
		iterator_Node=iterator_Node->nextPtr;
	}
	fputc('\n',fPtr);

	fprintf(fPtr, "超时数：%d；罚单：",OrderOverdue);
	iterator_Node=OrderOverdueList->nextPtr;
	while(iterator_Node)
	{
		fprintf(fPtr, "%d ",iterator_Node->num );
		iterator_Node=iterator_Node->nextPtr;
	}
	fputc('\n',fPtr);
	iterator_Node=NULL;
	Rider *iterator=RiderListHead->nextPtr;
	int i=0,j=0;
	while(iterator)
	{
		fprintf(fPtr, "骑手%d位置：%d，%d;停靠：",i++,iterator->pos->x,iterator->pos->y );

		for(j=0;j<2;++j)
			switch(iterator->stopby.status[j])
			{
				case NOSTOPPING:break;
				case RESTRAURANT:fprintf(fPtr, "餐馆 %d %d ",iterator->stopby.slot[j].x,iterator->stopby.slot[j].y );break;
				case CUSTOMER:fprintf(fPtr, "食客 %d %d ",iterator->stopby.slot[j].x,iterator->stopby.slot[j].y );break;
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
	while(iterator)
	{
		printf("%d %d %d %d %d %d\n",iterator->id,iterator->time,iterator->src->x,
		iterator->src->y,iterator->dst->x,iterator->dst->y );
		iterator=iterator->nextPtr;
	}
}


void debug_printAllThings()
{
	FILE *fPtr=fopen("debug.txt","a+");
	fprintf(fPtr, "当前时间：%d\n骑手数量：%d\n", CurrentTime,RiderCnt);
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
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
};//ͣ����״̬����û��ͣ����ͣ�����ǲ͹ݣ�ͣ������ʳ��
Node *OrderOverdueClipFile=NULL;//��¼��ʱ�Ķ��������ӡ��λ��
Node *OrderCompletedClipFile=NULL;//��¼��ɵĶ��������ӡ��λ��

int commandWindowInput();//����������
int fileInput(FILE *fPtr);//�ļ�����
int isFileAvailable();//�ж��ļ��Ƿ�ɶ�
int salesInput();//�ܵĶ��뺯��
int isFileEmpty();//�ж��ļ��Ƿ�Ϊ��
void updateFile();//��������ļ�
void sortOrder(Order *OrderListHead,int n);//������Ķ�����ʱ���������
void debug_printOrders();//��ӡ����
void debug_printRiders();//��ӡ���ֵ�������Ϣ

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
		//��д�����е�srcê�㣬src����ͨ��fromָ���ҳ��Լ����ԵĶ���
		currentPtr->dst->x=dst_x;
		currentPtr->dst->y=dst_y;
		currentPtr->dst->from=currentPtr;
		currentPtr->dst->nextPtr=NULL;
		//��дdstê��
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
		//����order������
		++OrderCnt;//��������
	}
	lastPtr->nextPtr=NULL;
	fclose(fPtr);
	//�����������β
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
	fprintf(fPtr, "ʱ�䣺%d\n",CurrentTime );
	fprintf(fPtr, "Ǯ��%d\n",Balance );
	fprintf(fPtr, "�ӵ�����%d\n",OrderTaken );

	fprintf(fPtr, "�������%d���ᵥ��",OrderCompleted );
	while(!isQueueEmpty(CompletedQueue))
		fprintf(fPtr, "%d ",outFromQueue(CompletedQueue) );
	fputc('\n',fPtr);

	fprintf(fPtr, "��ʱ����%d��������",OrderOverdue);
	while(!isQueueEmpty(OverdueQueue))
		fprintf(fPtr, "%d ",outFromQueue(OverdueQueue) );
	fputc('\n',fPtr);

	Rider *iterator=RiderListHead->nextPtr;
	Anchor *iterator_a,*prevPtr_1=NULL,*prevPtr_2=NULL;//prevPtrһ�Ͷ��ֱ𴢴����ͣ���Ĳ�ͬ��������
	int i=0,j=0,state_1=NOSTOPPING,state_2=NOSTOPPING;//stateһ�Ͷ��ֱ��ʾ��ͣ���������״̬
	while(iterator)
	{
		iterator_a=iterator->stopbyNodes->nextPtr;
		while(iterator_a)
		{
			if(!prevPtr_1)//���ֵ�һ��ͣ����
			{	
				if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y) state_1=RESTRAURANT;
				else state_1=CUSTOMER;
				prevPtr_1=iterator_a;
			}
			else 
			{
				if(!prevPtr_2 && (iterator_a->x!=prevPtr_1->x||iterator_a->y!=prevPtr_1->y))//���ֵڶ���ͣ�������������һ�㲻ͬ
				{
					prevPtr_2=iterator_a;
					if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y) state_2=RESTRAURANT;
					else state_2=CUSTOMER;
				}
				else if(!prevPtr_2 || iterator_a->x==prevPtr_1->x && iterator_a->y==prevPtr_1->y)//ͣ�������һ����ͬʱ
				{
					if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y && state_1==CUSTOMER) state_1=RESTCUST;
					if(iterator_a->x==iterator_a->from->dst->x && iterator_a->y==iterator_a->from->dst->y && state_1==RESTRAURANT) state_1=RESTCUST;
				}
				else//ͣ������ڶ�����ͬʱ
				{
					if(iterator_a->x==iterator_a->from->src->x && iterator_a->y==iterator_a->from->src->y && state_2==CUSTOMER) state_2=RESTCUST;
					if(iterator_a->x==iterator_a->from->dst->x && iterator_a->y==iterator_a->from->dst->y && state_2==RESTRAURANT) state_2=RESTCUST;
				}
			}
			iterator_a=iterator_a->nextPtr; 
		}
		fprintf(fPtr, "����%dλ�ã�%d��%d;ͣ����",i++,iterator->pos->x,iterator->pos->y);
		switch(state_1)
		{
			case NOSTOPPING:break;
			case RESTRAURANT:fprintf(fPtr, "�͹� %d %d ",prevPtr_1->x,prevPtr_1->y);break;
			case CUSTOMER:fprintf(fPtr, "ʳ�� %d %d ",prevPtr_1->x,prevPtr_1->y);break;
			case RESTCUST:fprintf(fPtr, "�Ϳ� %d %d ",prevPtr_1->x,prevPtr_1->y);break;
		}
		switch(state_2)
		{
			case NOSTOPPING:break;
			case RESTRAURANT:fprintf(fPtr, "�͹� %d %d ",prevPtr_2->x,prevPtr_2->y);break;
			case CUSTOMER:fprintf(fPtr, "ʳ�� %d %d ",prevPtr_2->x,prevPtr_2->y);break;
			case RESTCUST:fprintf(fPtr, "�Ϳ� %d %d ",prevPtr_2->x,prevPtr_2->y);break;
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
	//��Ҫ����isChanged��־λ����������֮��Ľڵ������Ӱ�죬Ҫ�ֿ�����
	if(n==0||n==1) return ;//���������ֻ��һ��Ԫ����ʲô�������
	Order *aPtr,*bPtr,*prevPtr;
	for(i=1;i<n;++i)//ð�����򣬹���n-1�ˣ�����Ϊ������û��ɶ����
	{
		prevPtr=NULL;//ÿ��һ�ˣ���ʼʱǰ�ýڵ���ΪNULL
		aPtr=OrderListHead->nextPtr;//a������ͷ��㣬��Ϊ�������п��ܻ�ı��������ڣ�ÿ����Ҫ���»�ȡ
		bPtr=aPtr->nextPtr;//b�ǵڶ����ڵ㣬ð����������������������
		isChanged=0;//��־λ�������벻����ʱ�������Ĳ���ʱ��һ����
		for(j=0;j<n-i;++j)//ð��ÿ�˼��n-i-1��
		{
			if(!prevPtr)//û��ǰ�ýڵ㣬��Ϊͷ���ʱ
			{
				if((aPtr->time)>(bPtr->time))//˵����Ҫ����ͷ���͵ڶ����ڵ�
				{
					OrderListHead->nextPtr=bPtr;//�������ڸĳ�b�ڵ�
					aPtr->nextPtr=bPtr->nextPtr;//a�ڵ�ĺ�̱��b�ڵ�ĺ��
					bPtr->nextPtr=aPtr;//��a�ڵ�ӵ�b����
					isChanged=1;//��־λ��˵�������˸ı�
				}
				if(isChanged)//��������ǰ�����ڵ�
				{
					bPtr=aPtr->nextPtr;//���Ǳ���aǰb��
					isChanged=0;
				}
				else//û�н����͸���������
				{
					bPtr=bPtr->nextPtr;
					aPtr=aPtr->nextPtr;
				}
				prevPtr=OrderListHead->nextPtr;//prevPtrָ���������
			}
			else//����ͷ��㣬�����м�������ڵ���ʱ
			{
				if((aPtr->time)>(bPtr->time))
				{
					isChanged=1;
					aPtr->nextPtr=bPtr->nextPtr;//a�ĺ�̸ĳ�b�ĺ��
					prevPtr->nextPtr=bPtr;//��b����ǰ��ڵ�
					bPtr->nextPtr=aPtr;//a�ڵ�ӵ�b�ڵ����
				}
				if(isChanged)//������
				{
					isChanged=0;
					prevPtr=prevPtr->nextPtr;//ǰ�ýڵ�������
					bPtr=aPtr->nextPtr;//����aǰb��
					aPtr=prevPtr->nextPtr;
				}
				else//����ָ�����������
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
	fprintf(fPtr, "Ǯ��%d����ǰʱ�䣺%d\n����������%d\n", Balance,CurrentTime,RiderCnt);
	Rider *iterator=RiderListHead->nextPtr;
	Anchor *way_iterator;
	int i=0;
	while(iterator)
	{
		fprintf(fPtr, "���֣�%d\n",i++ );
		fprintf(fPtr, "NodeCnt:%d\n",iterator->nodeCnt );
		fprintf(fPtr, "Pos:%d %d\n",iterator->pos->x,iterator->pos->y );
		way_iterator=iterator->path->nextPtr;
		fprintf(fPtr, "·����\n");
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
	//û�����ն��м�⣬����out֮ǰ���м��
	Node *frontCell=q->front;
	if(q->front==q->rear)//����ֻ��һ��Ԫ��
		q->front=q->rear=NULL;//ɾ��������ÿ�
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

typedef struct _anchor
{
	int x;
	int y;
	struct _order *from;//指向锚点来自订单及结构体
	struct _anchor *nextPtr;//节点域，用于将锚点串成链表
}Anchor;
//锚点：存储单个坐标，根据from可以找出其隶属的订单，用指针域可以串成链表

typedef struct _order
{
	int id;//订单id
	int time;//起始时间
	int state;//0表示未被派出可以派单，1表示已经被派出还未送达，2表示已送达 
	Anchor *src;//餐馆处的锚点
	Anchor *dst;//食客处的锚点
	struct _order *nextPtr;
}Order;
//订单：存储单个订单，借助指针域可以串成链表

typedef struct _rider
{
	int nodeCnt;//记录path中含有的锚点数，用于评估插入新锚点位置时创建数组
	Anchor *pos;//骑手当前位置锚点
	Anchor *path;//path为带头结点的链表的哨兵头结点
	Anchor *stopbyNodes;//骑手停靠的记录
	Anchor *lastStopby;
	struct _rider *nextPtr;
}Rider;
//骑手

typedef struct _node
{
	int num;
	struct _node* nextPtr;
}Node;
//数据链表：存储完成和超时的订单

typedef struct _qnode
{
	Node *front,*rear;
}QNode;

typedef QNode* Queue;

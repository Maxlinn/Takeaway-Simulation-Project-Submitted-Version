typedef struct _anchor
{
	int x;
	int y;
	struct _order *from;//指向锚点来自订单及结构体
	struct _anchor *nextPtr;//节点域，用于将锚点串成链表
}Anchor;

typedef struct _order
{
	int id;//订单id
	int time;
	Anchor *src;
	Anchor *dst;
	struct _order *nextPtr;
}Order;


typedef struct _stopby
{
	int status[4];
	Anchor slot[4];
}Stopby;

typedef struct _rider
{
	int nodeCnt;
	Anchor *pos;
	Anchor *path;//path为带头结点的链表的哨兵头结点
	Stopby stopby;
	struct _rider *nextPtr;//将骑手串成链表
}Rider;

typedef struct _image
{
	char* src;
	int len_x;
	int len_y;
}Image;

typedef struct _node
{
	int num;
	struct _node* nextPtr;
}Node;

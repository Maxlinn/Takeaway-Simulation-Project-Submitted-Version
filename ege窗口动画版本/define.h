typedef struct _anchor
{
	int x;
	int y;
	struct _order *from;//ָ��ê�����Զ������ṹ��
	struct _anchor *nextPtr;//�ڵ������ڽ�ê�㴮������
}Anchor;
//ê�㣺�洢�������꣬����from�����ҳ��������Ķ�������ָ������Դ�������

typedef struct _order
{
	int id;//����id
	int time;//��ʼʱ��
	int state;//0��ʾδ���ɳ������ɵ���1��ʾ�Ѿ����ɳ���δ�ʹ2��ʾ���ʹ� 
	Anchor *src;//�͹ݴ���ê��
	Anchor *dst;//ʳ�ʹ���ê��
	struct _order *nextPtr;
}Order;
//�������洢��������������ָ������Դ�������

typedef struct _rider
{
	int nodeCnt;//��¼path�к��е�ê��������������������ê��λ��ʱ��������
	Anchor *pos;//���ֵ�ǰλ��ê��
	Anchor *path;//pathΪ��ͷ����������ڱ�ͷ���
	Anchor *stopbyNodes;//����ͣ���ļ�¼
	Anchor *lastStopby;
	struct _rider *nextPtr;
}Rider;
//����

typedef struct _node
{
	int num;
	struct _node* nextPtr;
}Node;
//���������洢��ɺͳ�ʱ�Ķ���

typedef struct _qnode
{
	Node *front,*rear;
}QNode;

typedef QNode* Queue;

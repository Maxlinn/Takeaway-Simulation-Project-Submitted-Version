#include "graphics.h"
#include "define.h" 
#include <stdio.h>
enum ANIME_PARAMETERS{
	WLength=1280,WHeight=720,//���ڳߴ�
	BUTTON_XLen=160,BUTTON_YLen=40,//Ĭ�ϰ�ť�ߴ�
};


enum COLORS{
	LIGHT_GRAY=0x969696,DARK_GRAY=0x3F3F3F,
	BG_GRAY_INF=0XC8C8C8,BG_GRAY_SUP=0xE9E9E9,
	CREAM=0XFFFACD,
	NULL_COLOR=0xe3cf57,
};//������ɫ

enum WELCOME_BUTTONS
{
	FILEIN,COMMANDIN,ANIMATEIN,FILEOUT,COMMANDOUT,ANIMATEOUT,
	GO,
	BUTTON_CNT
};//��ӭ��Ļ�İ�ť

enum INPUT_FLAG//���뷽ʽ
{
	FILEINPUT,ANIMATEINPUT
};

enum OUTPUT_FLAG//�����ʽ
{
	FILEOUTPUT,ANIMATEOUTPUT,BOTHOUTPUT
};

typedef struct _fontset//������ַ������ļ�
{
	int length;//����ĳ���
	int height;//����ĸ߶ȣ����Ϊ0������Ӧ
	char* fontname;//��������
	color_t color;//������ɫ
	int mix_mode;//���ģʽ��OPAQUE��TRANSPARENT��ѡ
	color_t fontbkcolor;//����ı�����ɫ����OPAQUE����Ч
	int horizental_justify;//ˮƽ����ѡ��
	int vertical_justify;//��ֱ����ѡ��

}Fontset;//������������

typedef struct _button//��ť
{
	PIMAGE inactive;
	PIMAGE active;
	//��¼��ť������̬
	int state;
	//��¼��ť�Ĵ�С��λ��
	int xLen;
	int yLen;
	int left;
	int up;
	int right;
	int down;
	PIMAGE host;//Ҫ������IMG���� 
}Button;

extern int Balance;
//from main.c
extern int Time_Limit_Per_Order;
//from strategy.c
extern Order *OrderListHead;
//from main.c
extern Order *LastOrder;
//from main.c
extern int OrderCompleted; 
//from main.c
extern int input_flag;
//from main.c
extern int output_flag;
//from main.c
extern Order* OrderListHead;
//from main.c
extern int OrderCnt;
//from main.c
extern int OrderCompleted;
//from main.c
extern int OrderOverdue;
//from main.c
extern int isFileAvailable();
//from file.c
extern int isFileEmpty();
//from file.c
extern Rider* RiderListHead;
//from file.c
extern int CurrentTime,OrderTaken,OrderCompleted,OrderOverdue;
//from main.c

PIMAGE scene0,shop,house_gray,house_red,house_blue,screen,man,get,done,wulian,A,B,unselected;
//��Ҫ�����ͼ��
int left_pressed=0;
//����Ƿ���
mouse_msg m1,m2;
//������������¼�

Fontset f_headline={
	100,0,"��Բ",CYAN,TRANSPARENT,0,CENTER_TEXT,CENTER_TEXT
};

Fontset f_text={
	20,0,"����",BLACK,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_textwithbox={
	20,0,"����",WHITE,OPAQUE,BLACK,CENTER_TEXT,CENTER_TEXT
};

Fontset f_textbutton={
	20,0,"����",WHITE,TRANSPARENT,0,CENTER_TEXT,CENTER_TEXT
};

Fontset f_textwhite={
	20,0,"����",WHITE,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_info={
	40,0,"��Բ",BLACK,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_infocyan={
	40,0,"��Բ",CYAN,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_infored={
	40,0,"��Բ",RED,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

color_t alphasingle(color_t a, color_t b, color_t alpha);
//����ɫ�Ӻ���
color_t alphaMix(color_t inf, color_t sup, color_t alpha);
//����ɫ�Ӻ���
void gcRect(int x1,int y1,int x2,int y2,color_t inf,color_t sup,PIMAGE img=NULL);
//��(x1,y1)��(x2,y2)Χ�ɵľ��������inf��sup�Ľ���ɫ
void fontSwitcher(Fontset s,PIMAGE img=NULL);
//�л�Ԥ������
Button* createButton(char *str,int type=0,int size=20);
//typeΪ0�ʹ���Ĭ�Ϲ̶���С�İ�ť
//typeΪ1��ʾ�����������İ�ť 
void addActiveButtonImg(Button *b);
//���ɼ���İ�ťͼƬ����������ɫ����
int isClicked(mouse_msg m,Button *b);
//��ť�Ƿ񱻵��
void placeButton(Button *b,int x,int y,PIMAGE p=NULL);
//�Ѱ�ť�ŵ�p��(x,y)��
void updateButton(Button *b);
//�л���ť״̬
void setButtonToDefault(Button *b);
//����ť����Ϊδѡ��״̬
void delButton(Button *b);
//ɾ����ť
void welcome();
//��һ��
void loadPics();
//���빫��ͼƬ��Դ
void printScreen();
//���һ֡
void coordShift(int *x,int *y);
//���߼�����ת��Ϊ��Ļ����
#include "graphics.h"
#include "define.h" 
#include <stdio.h>
enum ANIME_PARAMETERS{
	WLength=1280,WHeight=720,//窗口尺寸
	BUTTON_XLen=160,BUTTON_YLen=40,//默认按钮尺寸
};


enum COLORS{
	LIGHT_GRAY=0x969696,DARK_GRAY=0x3F3F3F,
	BG_GRAY_INF=0XC8C8C8,BG_GRAY_SUP=0xE9E9E9,
	CREAM=0XFFFACD,
	NULL_COLOR=0xe3cf57,
};//常用颜色

enum WELCOME_BUTTONS
{
	FILEIN,COMMANDIN,ANIMATEIN,FILEOUT,COMMANDOUT,ANIMATEOUT,
	GO,
	BUTTON_CNT
};//欢迎屏幕的按钮

enum INPUT_FLAG//输入方式
{
	FILEINPUT,ANIMATEINPUT
};

enum OUTPUT_FLAG//输出方式
{
	FILEOUTPUT,ANIMATEOUTPUT,BOTHOUTPUT
};

typedef struct _fontset//保存的字符配置文件
{
	int length;//字体的长度
	int height;//字体的高度，如果为0则自适应
	char* fontname;//字体名称
	color_t color;//字体颜色
	int mix_mode;//混合模式，OPAQUE和TRANSPARENT可选
	color_t fontbkcolor;//字体的背景颜色，在OPAQUE下生效
	int horizental_justify;//水平对齐选项
	int vertical_justify;//竖直对齐选项

}Fontset;//保存字体设置

typedef struct _button//按钮
{
	PIMAGE inactive;
	PIMAGE active;
	//记录按钮的两种态
	int state;
	//记录按钮的大小和位置
	int xLen;
	int yLen;
	int left;
	int up;
	int right;
	int down;
	PIMAGE host;//要贴到的IMG对象 
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
//需要保存的图层
int left_pressed=0;
//左键是否按下
mouse_msg m1,m2;
//保存两次鼠标事件

Fontset f_headline={
	100,0,"幼圆",CYAN,TRANSPARENT,0,CENTER_TEXT,CENTER_TEXT
};

Fontset f_text={
	20,0,"黑体",BLACK,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_textwithbox={
	20,0,"黑体",WHITE,OPAQUE,BLACK,CENTER_TEXT,CENTER_TEXT
};

Fontset f_textbutton={
	20,0,"黑体",WHITE,TRANSPARENT,0,CENTER_TEXT,CENTER_TEXT
};

Fontset f_textwhite={
	20,0,"黑体",WHITE,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_info={
	40,0,"幼圆",BLACK,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_infocyan={
	40,0,"幼圆",CYAN,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

Fontset f_infored={
	40,0,"幼圆",RED,TRANSPARENT,0,LEFT_TEXT,CENTER_TEXT
};

color_t alphasingle(color_t a, color_t b, color_t alpha);
//渐变色子函数
color_t alphaMix(color_t inf, color_t sup, color_t alpha);
//渐变色子函数
void gcRect(int x1,int y1,int x2,int y2,color_t inf,color_t sup,PIMAGE img=NULL);
//在(x1,y1)和(x2,y2)围成的矩形内填充inf到sup的渐变色
void fontSwitcher(Fontset s,PIMAGE img=NULL);
//切换预设字体
Button* createButton(char *str,int type=0,int size=20);
//type为0就创建默认固定大小的按钮
//type为1表示创建被废弃的按钮 
void addActiveButtonImg(Button *b);
//生成激活的按钮图片，即加上蓝色遮罩
int isClicked(mouse_msg m,Button *b);
//按钮是否被点击
void placeButton(Button *b,int x,int y,PIMAGE p=NULL);
//把按钮放到p的(x,y)处
void updateButton(Button *b);
//切换按钮状态
void setButtonToDefault(Button *b);
//将按钮重置为未选择状态
void delButton(Button *b);
//删除按钮
void welcome();
//第一屏
void loadPics();
//载入公共图片资源
void printScreen();
//输出一帧
void coordShift(int *x,int *y);
//将逻辑坐标转换为屏幕坐标
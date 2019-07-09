#include "animate.h"

color_t alphasingle(color_t a, color_t b, color_t alpha)//渐变色子函数
{
	return (a * (0xFF - alpha) + b * alpha) >> 8;
}

color_t alphaMix(color_t inf, color_t sup, color_t alpha)//渐变色子函数
{
	color_t d=inf,s=sup;
	return (alphasingle(d&0xFF00FF, s&0xFF00FF, alpha) & 0xFF00FF)
		| (alphasingle(d&0xFF00, s&0xFF00, alpha) >> 8 << 8);
}

void gcRect(int x1,int y1,int x2,int y2,color_t inf,color_t sup,PIMAGE img)
//在(x1,y1)和(x2,y2)围成的矩形内填充inf到sup的渐变色
{
	///y2 > y1 by default
	double ratio=256/(y2-y1);
	int i;
	for(i=0;i<=y2-y1;++i)
	{
		setcolor(alphaMix(inf,sup,i*ratio),img);
		line(x1,y1+i,x2,y1+i,img);
	}
}

void fontSwitcher(Fontset s,PIMAGE img)
//切换预设字体，使用预设的字体类型大小颜色混合模式等
{
	setfont(s.length,s.height,s.fontname,img);
	setcolor(s.color,img);
	setbkmode(s.mix_mode,img);
	if(s.mix_mode==OPAQUE)
		setbkcolor(s.fontbkcolor,img);
	settextjustify(s.horizental_justify,s.vertical_justify,img);
}

Button* createButton(char *str,int type,int size)
//type为0就创建默认固定大小的按钮
//type为1表示创建被废弃的按钮 
{
	Button *b=(Button*)malloc(sizeof(Button));
	if(type<=1)//创建默认大小的按钮
	{
		PIMAGE img=newimage(BUTTON_XLen,BUTTON_YLen);

		//绘制按钮
		b->xLen=BUTTON_XLen;
		b->yLen=BUTTON_YLen;
		b->state=0;
		gcRect(0,0,BUTTON_XLen,BUTTON_YLen,LIGHT_GRAY,DARK_GRAY,img);

		//创建未激活时的按钮
		fontSwitcher(f_textbutton,img);
		outtextxy(BUTTON_XLen/2,BUTTON_YLen/2,str,img);
		b->inactive=img;
		
		//按钮添加按下时的图片
		addActiveButtonImg(b);
		if(type==1)
		{
			setcolor(WHITE,img);
			setlinewidth(5,img);
			line(0,0,BUTTON_XLen,BUTTON_YLen,img);
			//画删除线
			addActiveButtonImg(b);
		}
	}
	return b;
}

void addActiveButtonImg(Button *b)
{
	PIMAGE cover=newimage(b->xLen,b->yLen);
	//画蓝色遮罩
	setfillstyle(SOLID_FILL,(BLUE+CYAN)/2,cover);
	bar(0,0,b->xLen,b->yLen,cover);
	//混色
	putimage_alphablend(cover,b->inactive,0,0,0xAA,0,0,0,0);
	b->active=cover;
}

int isClicked(mouse_msg m,Button *b)
//按钮是否被点击
{
	if(b->left<=m.x&&m.x<=b->right&&b->up<=m.y&&m.y<=b->down)
		return 1;
	return 0;
}

void placeButton(Button *b,int x,int y,PIMAGE p)
//把按钮放到p的(x,y)处
{
	PIMAGE img=(b->state)?(b->active):(b->inactive);
	//按钮的当前状态决定贴上去的样式
	b->left=x;
	b->up=y;
	b->right=x+b->xLen;
	b->down=y+b->yLen;
	putimage(p,x,y,img);
	b->host=p; 
}

void updateButton(Button *b)
//切换按钮状态
{
	b->state=!(b->state);
	PIMAGE img=(b->state)?(b->active):(b->inactive);
	//取出新样式，打印
	putimage(b->host,b->left,b->up,img);
}

void setButtonToDefault(Button *b)
//将按钮重置为未选择状态
{
	if(!b->state)
		return;
	else
		updateButton(b);	
}

void delButton(Button *b)
//删除按钮
{
	delimage(b->inactive);
	delimage(b->active);
	free(b);
}

void welcome()
//第一屏
{	
	//创建窗口
	//必须先initgraph才能操作PIMAGE对象，不然会产生访问错误
	setinitmode(0); 
	initgraph(WLength,WHeight);
	setcaption("Real-Time Takeaway Process Simulation");
	setbkcolor(WHITE);
	
	//先全部画到scene0上,避免闪烁
	PIMAGE scene0=newimage(WLength,WHeight);
	setbkcolor(WHITE,scene0);
	//画上下的渐变色标题栏
	gcRect(0,0,WLength,80,LIGHT_GRAY,DARK_GRAY,scene0);
	gcRect(0,WHeight-80,WLength,WHeight,LIGHT_GRAY,DARK_GRAY,scene0);
	//画背景的渐变灰色
	gcRect(0,80,WLength,WHeight-80,BG_GRAY_INF,BG_GRAY_SUP,scene0);

	//主界面标题
	fontSwitcher(f_headline,scene0);
	outtextxy(630,220,"外卖订单模拟",scene0);
	fontSwitcher(f_text,scene0);
	outtextxy(450,290,"Real-Time Takeaway Process Simulation",scene0);
	//主界面选项
	fontSwitcher(f_text,scene0);
	setcolor(CYAN,scene0);
	outtextxy(70,350,"1.请选择输入方式（单选)：",scene0);
	outtextxy(70,450,"2.请选择输出方式（多选）:",scene0);
	//把袋鼠贴上去
	PIMAGE mt=newimage(400,200);
	getimage(mt,".\\assets\\mt.png");
	putimage_transparent(scene0,mt,20,100,BLACK);
	delimage(mt);
	//把叶子贴上去
	PIMAGE leaf=newimage(70,70);
	getimage(leaf,".\\assets\\leaf.png");
	putimage_transparent(scene0,leaf,20,640,BLACK);
	delimage(leaf);
	//写版权信息
	fontSwitcher(f_text,scene0);
	outtextxy(100,655,"Credit to Lupinus_Linn, BUPT",scene0);
	outtextxy(100,680,"Version 4.6.20 ,based on Easy Graphics Engine v19.01",scene0);
	outtextxy(100,705,"Any form of distribution is permitted. Don't claim it as your own.",scene0); 
	//贴到主屏幕上
	putimage(0,0,scene0);
	delimage(scene0);
	//创建欢迎界面的button数组
	Button **buttonArray=(Button**)malloc(sizeof(Button*)*BUTTON_CNT);
	buttonArray[FILEIN]=createButton("文件输入");
	buttonArray[COMMANDIN]=createButton("命令行输入",1);
	buttonArray[ANIMATEIN]=createButton("动画输入");
	buttonArray[FILEOUT]=createButton("文件输出");
	buttonArray[COMMANDOUT]=createButton("命令行输出",1);
	buttonArray[ANIMATEOUT]=createButton("动画输出");
	///手动创建GO按钮
	PIMAGE orangeWool=newimage(256,256);
	getimage(orangeWool,".\\assets\\ow.jpg");
	fontSwitcher(f_headline,orangeWool);
	setfont(60,0,"Calibri",orangeWool);
	outtextxy(128,128,"Link Start!",orangeWool);
	buttonArray[GO]=(Button*)malloc(sizeof(Button));
	buttonArray[GO]->state=0;
	buttonArray[GO]->inactive=orangeWool;
	buttonArray[GO]->xLen=256;
	buttonArray[GO]->yLen=256;
	addActiveButtonImg(buttonArray[GO]);
	//放置按钮到各个位置
	placeButton(buttonArray[FILEIN],100,375);
	placeButton(buttonArray[COMMANDIN],300,375);
	placeButton(buttonArray[ANIMATEIN],500,375);
	placeButton(buttonArray[FILEOUT],100,475);
	placeButton(buttonArray[COMMANDOUT],300,475);
	placeButton(buttonArray[ANIMATEOUT],500,475);
	placeButton(buttonArray[GO],760,325);

	int filein,commandin,animatein,fileout,commandout,animateout;
	mouse_msg mouse;
	int i;
	for(;is_run();Sleep(20))
	{
		filein=buttonArray[FILEIN]->state;
		commandin=buttonArray[COMMANDIN]->state;
		animatein=buttonArray[ANIMATEIN]->state;
		fileout=buttonArray[FILEOUT]->state;
		commandout=buttonArray[COMMANDOUT]->state;
		animateout=buttonArray[ANIMATEOUT]->state;

		if(buttonArray[GO]->state)//如果确认按钮被点击
		{
			if(commandin||commandout)
				MessageBox(getHWnd(),"命令行输入/输出在该版中被废除，请重新选择","选择错误",MB_OK);
			else if(filein&&animatein)
				MessageBox(getHWnd(),"输入方式只能选择一种，请重新选择","选择错误",MB_OK);
			else if(!filein&&!animatein)
				MessageBox(getHWnd(),"请选择至少一种输入方式，请重新选择","选择错误",MB_OK);
			else if(!fileout&&!animateout)
				MessageBox(getHWnd(),"请选择至少一种输出方式，请重新选择","选择错误",MB_OK);
			else if(filein&&!isFileAvailable())
				MessageBox(getHWnd(),"当前文件不可用，请检查文件是否放好？无法访问当前目录下sales.txt","选择错误",MB_OK);
			else if(filein&&isFileEmpty())
				MessageBox(getHWnd(),"sales.txt为空，请检查订单是否已经输入了文件？","选择错误",MB_OK);
			else//选择完成
			{
				input_flag=filein?FILEINPUT:ANIMATEINPUT;
				if(fileout&&animateout)
					output_flag=BOTHOUTPUT;
				else if(fileout)
					output_flag=FILEOUT;
				else output_flag=ANIMATEOUT;
				break;
			}
			//重置所有按钮
			for(i=0;i<BUTTON_CNT;++i)
				setButtonToDefault(buttonArray[i]);
		}

		//检测鼠标操作
		while(mousemsg())
			mouse=getmouse();
		if(mouse.is_down() &&  (mouse=getmouse()).is_up())//检测到一次点击
			{
				for(int i=0;i<BUTTON_CNT;++i)
					if(isClicked(mouse,buttonArray[i]))
						updateButton(buttonArray[i]);
			}	
	}
	cleardevice();
	for(i=0;i<BUTTON_CNT;++i)
		delButton(buttonArray[i]);//删除按钮
}

void loadPics()//载入公共图片资源
{
	scene0=newimage(WLength,WHeight) ;
	setbkcolor(WHITE,scene0);
	screen=newimage(WLength,WHeight) ;
	setbkcolor(WHITE,screen);
	//贴背景木质纹理 
	PIMAGE bg=newimage(WLength,WHeight);
	getimage(bg,".\\assets\\bg.jpg");
	putimage(scene0,0,0,bg);
	delimage(bg);
	//贴地图，之后scene0会作为每层的基本图层，不要再修改scene0了
	PIMAGE map=newimage(630,630);
	getimage(map,".\\assets\\map.jpg");
	putimage(scene0,600,30,map);
	delimage(map);
	//贴布告栏	
	PIMAGE note=newimage(570,440);
	getimage(note,".\\assets\\note.png");
	putimage_withalpha(scene0,note,10,230);
	delimage(note);
	//载入其他图片
	shop=newimage(70,70);
	getimage(shop,".\\assets\\shop.png");
	
	house_gray=newimage(70,70);
	getimage(house_gray,".\\assets\\house0.png");
	
	house_red=newimage(70,70);
	getimage(house_red,".\\assets\\house1.png");
	
	house_blue=newimage(70,70);
	getimage(house_blue,".\\assets\\house2.png");
	
	man=newimage(70,70);
	getimage(man,".\\assets\\rider1.png");
	

	get=newimage(40,20);
	getimage(get,".\\assets\\get.png");

	done=newimage(40,20);
	getimage(done,".\\assets\\done.png");

	wulian=newimage(400,400);
	getimage(wulian,".\\assets\\wulian.png");

	A=newimage(35,35);
	getimage(A,".\\assets\\A.png");

	B=newimage(35,35);
	getimage(B,".\\assets\\B.png");

	unselected=newimage(35,35);
	getimage(unselected,".\\assets\\unselected.png");

}

void updateScreen()
{
	putimage(screen,0,0,scene0);
	//清空屏幕，仅保留背景，便利贴和空地图

	int special_building[17][17]={0};
	//有特殊建筑的地方做标记

	Rider *iterator=RiderListHead->nextPtr;
	Anchor *iterator_a;
	int x,y;
	int cnt;	
	
	while(iterator)//读取骑手信息
	{
		//打印途经房子
		iterator_a=iterator->path->nextPtr;
		while(iterator_a)
		{
			x=iterator_a->x;y=iterator_a->y;
			special_building[x][y]=1;
			//遍历骑手路径上的点，标记一下等下不打印空房子了
			coordShift(&x,&y);
			if(iterator_a->from->src->x==iterator_a->x && iterator_a->from->src->y==iterator_a->y)
				//说明是src
				putimage_withalpha(screen,shop,x,y);
			else
				//说明是dst
				putimage_withalpha(screen,house_blue,x,y);
			
			iterator_a=iterator_a->nextPtr;
		}

		iterator_a=iterator->stopbyNodes->nextPtr;
		while(iterator_a)//把要停靠的点也先打印出来
		{
			x=iterator_a->x;y=iterator_a->y;
			special_building[x][y]=1;
			//遍历骑手路径上的点，标记一下等下不打印空房子了
			coordShift(&x,&y);
			if(iterator_a->from->src->x==iterator_a->x && iterator_a->from->src->y==iterator_a->y)
				//说明是src
				{
					putimage_withalpha(screen,shop,x,y);
					putimage_withalpha(screen,get,x+5,y);
				}
			else
				//说明是dst
				{
					putimage_withalpha(screen,house_blue,x,y);
					putimage_withalpha(screen,done,x+5,y);
				}
			
			iterator_a=iterator_a->nextPtr;
		}

		iterator=iterator->nextPtr;
	}

	int i,j;
	for(i=0;i<17;++i)
		for(j=0;j<17;++j)
			if(i%2==0&&j%2==0)//说明是该打印房子的位置
				if(!special_building[i][j])//说明是空房子
				{
					x=i;y=j;
					coordShift(&x,&y);
					putimage_withalpha(screen,house_gray,x,y);
				}
	//打印文字
	fontSwitcher(f_info,screen);
	char s[50];
	sprintf(s,"当前时间：%d",CurrentTime);
	outtextxy(20,30,s,screen);
	
	sprintf(s,"接单数：%d",OrderTaken);
	outtextxy(20,80,s,screen);
	
	sprintf(s,"完成数：%d",OrderCompleted);
	outtextxy(20,130,s,screen);
	
	sprintf(s,"超时数：%d",OrderOverdue);
	outtextxy(20,180,s,screen); 
	
	sprintf(s,"账户余额：%d",Balance);
	outtextxy(20,230,s,screen);

	//打印布告栏
	Order *iterator_o=OrderListHead->nextPtr;
	if(OrderCnt==OrderCompleted)
	{
		fontSwitcher(f_infocyan,screen);
		switch(input_flag)
		{
			case FILEINPUT:outtextxy(12,300,"订单已处理完毕！",screen);
				outtextxy(12,350,"请右上角关闭程序",screen);
				break;
			case ANIMATEINPUT:
				outtextxy(12,300,"添加的订单已处理完毕！",screen);
				outtextxy(12,350,"请拖动继续添加",screen);
				outtextxy(12,400,"或右上角关闭程序",screen);
				break;
		}	
	}
	else
	{
		fontSwitcher(f_text,screen);
		for(i=0;i<16&&iterator_o;iterator_o=iterator_o->nextPtr)
		{	
			if(iterator_o->state==1)
				{
					sprintf(s,"订单号:%2d|餐馆:(%2d,%2d)|食客:(%2d,%2d)|起始:%2d|剩余:%2d",iterator_o->id,iterator_o->src->x,
					iterator_o->src->y,iterator_o->dst->x,iterator_o->dst->y,iterator_o->time,Time_Limit_Per_Order-(CurrentTime-iterator_o->time));
					outtextxy(12,270+22*i,s,screen);
					++i; 
				}
		}
	}

	iterator=RiderListHead->nextPtr;
	i=0;
	PIMAGE tmpRider=newimage(70,70);//暂时保存有序号的骑手
	
	while(iterator)
	{
		x=iterator->pos->x;y=iterator->pos->y;
		coordShift(&x,&y);
		putimage(tmpRider,0,0,man);
		//重置临时图片
		fontSwitcher(f_infored,tmpRider);
		//写上骑手序号
		sprintf(s,"%d",i++);
		outtextxy(20,20,s,tmpRider);
		putimage_transparent(screen,tmpRider,x,y,getpixel(0,0,tmpRider));
		iterator=iterator->nextPtr;
	}

	//鼠标拖动加上效果
	if(left_pressed)
	{
		int psrcx=(m1.x-600)/70;psrcx*=2;
		int psrcy=(m1.y-30)/70;psrcy*=2;
		int pdstx=(m2.x-600)/70;pdstx*=2;
		int pdsty=(m2.y-30)/70;pdsty*=2;

		coordShift(&psrcx,&psrcy);
		coordShift(&pdstx,&pdsty);

		putimage_withalpha(screen,A,psrcx,psrcy);
		if(!(m2.x==0&&m2.y==0))
		//第一次拖动还未到达时，m2为空，此时不要打印
		putimage_withalpha(screen,B,pdstx,pdsty);
	}
}

void printScreen()
//输出一帧
{
	putimage(0,0,screen);
}

void coordShift(int *x,int *y)
//将逻辑坐标转换成打印位置
{
	if((*x)%2==0&&(*y)%2==0)//双偶数，说明是房子
	{
		*x=610+(*x/2)*70;
		*y=40+(*y/2)*70;
	} 
	else if((*x)%2!=0&&(*y)%2==0)//说明在竖着的道路上
	{
		*x=640+(*x)/2*70;
		*y=40+(*y)/2*70;
	}
	else//说明在横着的道路上 
	{
		*x=610+(*x)/2*70;
		*y=50+(*y)/2*70;
	 } 
}

void mouseDealer()
//鼠标处理线程
{
	int srcx,srcy,dstx,dsty;
	while(1)
	{
		left_pressed=0;
		while(!(m1=getmouse()).is_down());//等待鼠标左键按下
		left_pressed=1;

		srcx=(m1.x-600)/70;srcx*=2;
		srcy=(m1.y-30)/70;srcy*=2;

		if(m1.is_down())
		{
			while(!(m2=getmouse()).is_up());//等待鼠标左键弹起
			if(!(600<=m1.x&&m1.x<=600+630 && 30<=m1.y&&m1.y<=30+630 && 600<=m2.x&&m2.x<=600+630 && 30<=m2.y&&m2.y<=30+630))
				continue;//如果两次鼠标输入之间有一次的不再范围内，就舍弃这一次输入 
			dstx=(m2.x-600)/70;dstx*=2;
			dsty=(m2.y-30)/70;dsty*=2;

			//添加新订单
			Order *currentPtr=(Order*)malloc(sizeof(Order));
			currentPtr=(Order*)malloc(sizeof(Order));

			currentPtr->id=OrderCnt++;
			currentPtr->time=CurrentTime;
			currentPtr->state=0;

			currentPtr->src=(Anchor*)malloc(sizeof(Anchor));
			currentPtr->dst=(Anchor*)malloc(sizeof(Anchor));

			currentPtr->src->x=srcx;
			currentPtr->src->y=srcy;
			currentPtr->src->from=currentPtr;
			currentPtr->src->nextPtr=NULL;
			//填写订单中的src锚点，src可以通过from指针找出自己来自的订单
			currentPtr->dst->x=dstx;
			currentPtr->dst->y=dsty;
			currentPtr->dst->from=currentPtr;
			currentPtr->dst->nextPtr=NULL;

			currentPtr->nextPtr=NULL;

			if(!LastOrder)
			{
				OrderListHead->nextPtr=currentPtr;
				LastOrder=currentPtr;
			}
			else
			{
				LastOrder->nextPtr=currentPtr;
				LastOrder=currentPtr;
			}
		}
		Sleep(20);
	}
}

void broken()
//破产后的效果
{
	fontSwitcher(f_headline,NULL);
	putimage_withalpha(NULL,wulian,10,200);
	outtextxy(420,200,"破产了",NULL);
}


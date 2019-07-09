#include "animate.h"

color_t alphasingle(color_t a, color_t b, color_t alpha)//����ɫ�Ӻ���
{
	return (a * (0xFF - alpha) + b * alpha) >> 8;
}

color_t alphaMix(color_t inf, color_t sup, color_t alpha)//����ɫ�Ӻ���
{
	color_t d=inf,s=sup;
	return (alphasingle(d&0xFF00FF, s&0xFF00FF, alpha) & 0xFF00FF)
		| (alphasingle(d&0xFF00, s&0xFF00, alpha) >> 8 << 8);
}

void gcRect(int x1,int y1,int x2,int y2,color_t inf,color_t sup,PIMAGE img)
//��(x1,y1)��(x2,y2)Χ�ɵľ��������inf��sup�Ľ���ɫ
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
//�л�Ԥ�����壬ʹ��Ԥ����������ʹ�С��ɫ���ģʽ��
{
	setfont(s.length,s.height,s.fontname,img);
	setcolor(s.color,img);
	setbkmode(s.mix_mode,img);
	if(s.mix_mode==OPAQUE)
		setbkcolor(s.fontbkcolor,img);
	settextjustify(s.horizental_justify,s.vertical_justify,img);
}

Button* createButton(char *str,int type,int size)
//typeΪ0�ʹ���Ĭ�Ϲ̶���С�İ�ť
//typeΪ1��ʾ�����������İ�ť 
{
	Button *b=(Button*)malloc(sizeof(Button));
	if(type<=1)//����Ĭ�ϴ�С�İ�ť
	{
		PIMAGE img=newimage(BUTTON_XLen,BUTTON_YLen);

		//���ư�ť
		b->xLen=BUTTON_XLen;
		b->yLen=BUTTON_YLen;
		b->state=0;
		gcRect(0,0,BUTTON_XLen,BUTTON_YLen,LIGHT_GRAY,DARK_GRAY,img);

		//����δ����ʱ�İ�ť
		fontSwitcher(f_textbutton,img);
		outtextxy(BUTTON_XLen/2,BUTTON_YLen/2,str,img);
		b->inactive=img;
		
		//��ť��Ӱ���ʱ��ͼƬ
		addActiveButtonImg(b);
		if(type==1)
		{
			setcolor(WHITE,img);
			setlinewidth(5,img);
			line(0,0,BUTTON_XLen,BUTTON_YLen,img);
			//��ɾ����
			addActiveButtonImg(b);
		}
	}
	return b;
}

void addActiveButtonImg(Button *b)
{
	PIMAGE cover=newimage(b->xLen,b->yLen);
	//����ɫ����
	setfillstyle(SOLID_FILL,(BLUE+CYAN)/2,cover);
	bar(0,0,b->xLen,b->yLen,cover);
	//��ɫ
	putimage_alphablend(cover,b->inactive,0,0,0xAA,0,0,0,0);
	b->active=cover;
}

int isClicked(mouse_msg m,Button *b)
//��ť�Ƿ񱻵��
{
	if(b->left<=m.x&&m.x<=b->right&&b->up<=m.y&&m.y<=b->down)
		return 1;
	return 0;
}

void placeButton(Button *b,int x,int y,PIMAGE p)
//�Ѱ�ť�ŵ�p��(x,y)��
{
	PIMAGE img=(b->state)?(b->active):(b->inactive);
	//��ť�ĵ�ǰ״̬��������ȥ����ʽ
	b->left=x;
	b->up=y;
	b->right=x+b->xLen;
	b->down=y+b->yLen;
	putimage(p,x,y,img);
	b->host=p; 
}

void updateButton(Button *b)
//�л���ť״̬
{
	b->state=!(b->state);
	PIMAGE img=(b->state)?(b->active):(b->inactive);
	//ȡ������ʽ����ӡ
	putimage(b->host,b->left,b->up,img);
}

void setButtonToDefault(Button *b)
//����ť����Ϊδѡ��״̬
{
	if(!b->state)
		return;
	else
		updateButton(b);	
}

void delButton(Button *b)
//ɾ����ť
{
	delimage(b->inactive);
	delimage(b->active);
	free(b);
}

void welcome()
//��һ��
{	
	//��������
	//������initgraph���ܲ���PIMAGE���󣬲�Ȼ��������ʴ���
	setinitmode(0); 
	initgraph(WLength,WHeight);
	setcaption("Real-Time Takeaway Process Simulation");
	setbkcolor(WHITE);
	
	//��ȫ������scene0��,������˸
	PIMAGE scene0=newimage(WLength,WHeight);
	setbkcolor(WHITE,scene0);
	//�����µĽ���ɫ������
	gcRect(0,0,WLength,80,LIGHT_GRAY,DARK_GRAY,scene0);
	gcRect(0,WHeight-80,WLength,WHeight,LIGHT_GRAY,DARK_GRAY,scene0);
	//�������Ľ����ɫ
	gcRect(0,80,WLength,WHeight-80,BG_GRAY_INF,BG_GRAY_SUP,scene0);

	//���������
	fontSwitcher(f_headline,scene0);
	outtextxy(630,220,"��������ģ��",scene0);
	fontSwitcher(f_text,scene0);
	outtextxy(450,290,"Real-Time Takeaway Process Simulation",scene0);
	//������ѡ��
	fontSwitcher(f_text,scene0);
	setcolor(CYAN,scene0);
	outtextxy(70,350,"1.��ѡ�����뷽ʽ����ѡ)��",scene0);
	outtextxy(70,450,"2.��ѡ�������ʽ����ѡ��:",scene0);
	//�Ѵ�������ȥ
	PIMAGE mt=newimage(400,200);
	getimage(mt,".\\assets\\mt.png");
	putimage_transparent(scene0,mt,20,100,BLACK);
	delimage(mt);
	//��Ҷ������ȥ
	PIMAGE leaf=newimage(70,70);
	getimage(leaf,".\\assets\\leaf.png");
	putimage_transparent(scene0,leaf,20,640,BLACK);
	delimage(leaf);
	//д��Ȩ��Ϣ
	fontSwitcher(f_text,scene0);
	outtextxy(100,655,"Credit to Lupinus_Linn, BUPT",scene0);
	outtextxy(100,680,"Version 4.6.20 ,based on Easy Graphics Engine v19.01",scene0);
	outtextxy(100,705,"Any form of distribution is permitted. Don't claim it as your own.",scene0); 
	//��������Ļ��
	putimage(0,0,scene0);
	delimage(scene0);
	//������ӭ�����button����
	Button **buttonArray=(Button**)malloc(sizeof(Button*)*BUTTON_CNT);
	buttonArray[FILEIN]=createButton("�ļ�����");
	buttonArray[COMMANDIN]=createButton("����������",1);
	buttonArray[ANIMATEIN]=createButton("��������");
	buttonArray[FILEOUT]=createButton("�ļ����");
	buttonArray[COMMANDOUT]=createButton("���������",1);
	buttonArray[ANIMATEOUT]=createButton("�������");
	///�ֶ�����GO��ť
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
	//���ð�ť������λ��
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

		if(buttonArray[GO]->state)//���ȷ�ϰ�ť�����
		{
			if(commandin||commandout)
				MessageBox(getHWnd(),"����������/����ڸð��б��ϳ���������ѡ��","ѡ�����",MB_OK);
			else if(filein&&animatein)
				MessageBox(getHWnd(),"���뷽ʽֻ��ѡ��һ�֣�������ѡ��","ѡ�����",MB_OK);
			else if(!filein&&!animatein)
				MessageBox(getHWnd(),"��ѡ������һ�����뷽ʽ��������ѡ��","ѡ�����",MB_OK);
			else if(!fileout&&!animateout)
				MessageBox(getHWnd(),"��ѡ������һ�������ʽ��������ѡ��","ѡ�����",MB_OK);
			else if(filein&&!isFileAvailable())
				MessageBox(getHWnd(),"��ǰ�ļ������ã������ļ��Ƿ�źã��޷����ʵ�ǰĿ¼��sales.txt","ѡ�����",MB_OK);
			else if(filein&&isFileEmpty())
				MessageBox(getHWnd(),"sales.txtΪ�գ����鶩���Ƿ��Ѿ��������ļ���","ѡ�����",MB_OK);
			else//ѡ�����
			{
				input_flag=filein?FILEINPUT:ANIMATEINPUT;
				if(fileout&&animateout)
					output_flag=BOTHOUTPUT;
				else if(fileout)
					output_flag=FILEOUT;
				else output_flag=ANIMATEOUT;
				break;
			}
			//�������а�ť
			for(i=0;i<BUTTON_CNT;++i)
				setButtonToDefault(buttonArray[i]);
		}

		//���������
		while(mousemsg())
			mouse=getmouse();
		if(mouse.is_down() &&  (mouse=getmouse()).is_up())//��⵽һ�ε��
			{
				for(int i=0;i<BUTTON_CNT;++i)
					if(isClicked(mouse,buttonArray[i]))
						updateButton(buttonArray[i]);
			}	
	}
	cleardevice();
	for(i=0;i<BUTTON_CNT;++i)
		delButton(buttonArray[i]);//ɾ����ť
}

void loadPics()//���빫��ͼƬ��Դ
{
	scene0=newimage(WLength,WHeight) ;
	setbkcolor(WHITE,scene0);
	screen=newimage(WLength,WHeight) ;
	setbkcolor(WHITE,screen);
	//������ľ������ 
	PIMAGE bg=newimage(WLength,WHeight);
	getimage(bg,".\\assets\\bg.jpg");
	putimage(scene0,0,0,bg);
	delimage(bg);
	//����ͼ��֮��scene0����Ϊÿ��Ļ���ͼ�㣬��Ҫ���޸�scene0��
	PIMAGE map=newimage(630,630);
	getimage(map,".\\assets\\map.jpg");
	putimage(scene0,600,30,map);
	delimage(map);
	//��������	
	PIMAGE note=newimage(570,440);
	getimage(note,".\\assets\\note.png");
	putimage_withalpha(scene0,note,10,230);
	delimage(note);
	//��������ͼƬ
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
	//�����Ļ���������������������Ϳյ�ͼ

	int special_building[17][17]={0};
	//�����⽨���ĵط������

	Rider *iterator=RiderListHead->nextPtr;
	Anchor *iterator_a;
	int x,y;
	int cnt;	
	
	while(iterator)//��ȡ������Ϣ
	{
		//��ӡ;������
		iterator_a=iterator->path->nextPtr;
		while(iterator_a)
		{
			x=iterator_a->x;y=iterator_a->y;
			special_building[x][y]=1;
			//��������·���ϵĵ㣬���һ�µ��²���ӡ�շ�����
			coordShift(&x,&y);
			if(iterator_a->from->src->x==iterator_a->x && iterator_a->from->src->y==iterator_a->y)
				//˵����src
				putimage_withalpha(screen,shop,x,y);
			else
				//˵����dst
				putimage_withalpha(screen,house_blue,x,y);
			
			iterator_a=iterator_a->nextPtr;
		}

		iterator_a=iterator->stopbyNodes->nextPtr;
		while(iterator_a)//��Ҫͣ���ĵ�Ҳ�ȴ�ӡ����
		{
			x=iterator_a->x;y=iterator_a->y;
			special_building[x][y]=1;
			//��������·���ϵĵ㣬���һ�µ��²���ӡ�շ�����
			coordShift(&x,&y);
			if(iterator_a->from->src->x==iterator_a->x && iterator_a->from->src->y==iterator_a->y)
				//˵����src
				{
					putimage_withalpha(screen,shop,x,y);
					putimage_withalpha(screen,get,x+5,y);
				}
			else
				//˵����dst
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
			if(i%2==0&&j%2==0)//˵���Ǹô�ӡ���ӵ�λ��
				if(!special_building[i][j])//˵���ǿշ���
				{
					x=i;y=j;
					coordShift(&x,&y);
					putimage_withalpha(screen,house_gray,x,y);
				}
	//��ӡ����
	fontSwitcher(f_info,screen);
	char s[50];
	sprintf(s,"��ǰʱ�䣺%d",CurrentTime);
	outtextxy(20,30,s,screen);
	
	sprintf(s,"�ӵ�����%d",OrderTaken);
	outtextxy(20,80,s,screen);
	
	sprintf(s,"�������%d",OrderCompleted);
	outtextxy(20,130,s,screen);
	
	sprintf(s,"��ʱ����%d",OrderOverdue);
	outtextxy(20,180,s,screen); 
	
	sprintf(s,"�˻���%d",Balance);
	outtextxy(20,230,s,screen);

	//��ӡ������
	Order *iterator_o=OrderListHead->nextPtr;
	if(OrderCnt==OrderCompleted)
	{
		fontSwitcher(f_infocyan,screen);
		switch(input_flag)
		{
			case FILEINPUT:outtextxy(12,300,"�����Ѵ�����ϣ�",screen);
				outtextxy(12,350,"�����Ͻǹرճ���",screen);
				break;
			case ANIMATEINPUT:
				outtextxy(12,300,"��ӵĶ����Ѵ�����ϣ�",screen);
				outtextxy(12,350,"���϶��������",screen);
				outtextxy(12,400,"�����Ͻǹرճ���",screen);
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
					sprintf(s,"������:%2d|�͹�:(%2d,%2d)|ʳ��:(%2d,%2d)|��ʼ:%2d|ʣ��:%2d",iterator_o->id,iterator_o->src->x,
					iterator_o->src->y,iterator_o->dst->x,iterator_o->dst->y,iterator_o->time,Time_Limit_Per_Order-(CurrentTime-iterator_o->time));
					outtextxy(12,270+22*i,s,screen);
					++i; 
				}
		}
	}

	iterator=RiderListHead->nextPtr;
	i=0;
	PIMAGE tmpRider=newimage(70,70);//��ʱ��������ŵ�����
	
	while(iterator)
	{
		x=iterator->pos->x;y=iterator->pos->y;
		coordShift(&x,&y);
		putimage(tmpRider,0,0,man);
		//������ʱͼƬ
		fontSwitcher(f_infored,tmpRider);
		//д���������
		sprintf(s,"%d",i++);
		outtextxy(20,20,s,tmpRider);
		putimage_transparent(screen,tmpRider,x,y,getpixel(0,0,tmpRider));
		iterator=iterator->nextPtr;
	}

	//����϶�����Ч��
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
		//��һ���϶���δ����ʱ��m2Ϊ�գ���ʱ��Ҫ��ӡ
		putimage_withalpha(screen,B,pdstx,pdsty);
	}
}

void printScreen()
//���һ֡
{
	putimage(0,0,screen);
}

void coordShift(int *x,int *y)
//���߼�����ת���ɴ�ӡλ��
{
	if((*x)%2==0&&(*y)%2==0)//˫ż����˵���Ƿ���
	{
		*x=610+(*x/2)*70;
		*y=40+(*y/2)*70;
	} 
	else if((*x)%2!=0&&(*y)%2==0)//˵�������ŵĵ�·��
	{
		*x=640+(*x)/2*70;
		*y=40+(*y)/2*70;
	}
	else//˵���ں��ŵĵ�·�� 
	{
		*x=610+(*x)/2*70;
		*y=50+(*y)/2*70;
	 } 
}

void mouseDealer()
//��괦���߳�
{
	int srcx,srcy,dstx,dsty;
	while(1)
	{
		left_pressed=0;
		while(!(m1=getmouse()).is_down());//�ȴ�����������
		left_pressed=1;

		srcx=(m1.x-600)/70;srcx*=2;
		srcy=(m1.y-30)/70;srcy*=2;

		if(m1.is_down())
		{
			while(!(m2=getmouse()).is_up());//�ȴ�����������
			if(!(600<=m1.x&&m1.x<=600+630 && 30<=m1.y&&m1.y<=30+630 && 600<=m2.x&&m2.x<=600+630 && 30<=m2.y&&m2.y<=30+630))
				continue;//��������������֮����һ�εĲ��ٷ�Χ�ڣ���������һ������ 
			dstx=(m2.x-600)/70;dstx*=2;
			dsty=(m2.y-30)/70;dsty*=2;

			//����¶���
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
			//��д�����е�srcê�㣬src����ͨ��fromָ���ҳ��Լ����ԵĶ���
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
//�Ʋ����Ч��
{
	fontSwitcher(f_headline,NULL);
	putimage_withalpha(NULL,wulian,10,200);
	outtextxy(420,200,"�Ʋ���",NULL);
}


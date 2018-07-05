// cpp: implementation of the Snake class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "snake.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <time.h>
#include <windows.h>

extern HWND hwnd; 

#define FONTHEITH 15
using namespace std;

const int dx[4] = {0,1,0,-1};//0 UP 1 RIGHT 2 DOWN 3 LEFT
const int dy[4] = {-1,0,1,0};

int sx;				// ��¼��ʼ��λ��
int sy;				
int d;				// �ñ������ڿ��������ܺ���

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void PutOutText(char* lpsz,int X,int Y,unsigned long fontcolor){
	HDC hdc;
	HFONT hF_black;	//��������������
	TEXTMETRIC tm;			//����һ��TEXTMETRIC�ṹ�����Լ�¼������Ϣ

	//HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	hdc=GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	hF_black=CreateFont		//�����Զ�������
			(
				FONTHEITH,				//����ĸ߶�
				0,				//��ϵͳ���ݸ߿��ѡȡ������ѿ��ֵ
				0,				//�ı�����б��Ϊ0����ʾˮƽ
				0,				//�������б��Ϊ0
				FW_NORMAL,		//����Ĵֶȣ�FW_HEAVYΪ���
				0,				//��б����
				0,				//���»���
				0,				//��ɾ����
				GB2312_CHARSET,	//��ʾ���õ��ַ���ΪANSI_CHARSET
				OUT_DEFAULT_PRECIS,	//�������Ϊȱʡ����
				CLIP_DEFAULT_PRECIS,	//���þ���Ϊȱʡ����
				DEFAULT_QUALITY,		//�������Ϊȱʡֵ
				DEFAULT_PITCH|FF_DONTCARE,//�ּ�������ϵ��ʹ��ȱʡֵ
				"������"				//��������
				);
		SetTextColor(hdc,fontcolor);	//�����ı���ɫΪ��ɫ
		SetBkColor(hdc,RGB(0,0,0));
		SelectObject(hdc,hF_black);		//���Զ�������ѡ���豸����
		GetTextMetrics(hdc,&tm);		//��ȡ�������Ϣ����д��tm�ṹ��

		TextOut(hdc,X,Y,lpsz,strlen(lpsz));	//ʹ�õ�ǰ��������ı�
		Y=Y+tm.tmHeight+tm.tmExternalLeading;//���㻻��ʱ��һ���ı����������
	
		ReleaseDC(hwnd,hdc);
		DeleteObject(hF_black);		//ɾ���Զ���������
}

Snake::Snake()
{
	direct = 1;		//����
	nGameState=1;	
	score=0;

	tot=0;			//�Ե�����Ʒ��
	level=1;		//��ǰ�ȼ�
	per=1;			//ÿ����Ʒ�ĵ÷�
	speed=200;		//��ʼ�ٶ�
	d=0;
	curr=2;			//��ǰ�߳���
	sx=sy=4;		//��ʼ����
	
	memset(data,'-1',sizeof(data));
	data[0].x=sx;
	data[0].y=sy;
	data[1].x=sx-1;
	data[1].y=sy;

	food.x=9;
	food.y=9;
	food.special=0;

	UnDrawSnake();
	DrawSnake();
	DrawFood();
}

Snake::~Snake()
{

}

void Snake::DrawFood()
{
	 //HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	 HDC hDC=GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	 HPEN hPen; //����
	 HBRUSH hBrush;
 
	 hPen=CreatePen(PS_SOLID, 0, 0xff00ff);
	 if (food.special == 0) 
		 hBrush=CreateSolidBrush(0x000000); 
	 else
		 hBrush=CreateSolidBrush(0x0000ff);
	 
	 HPEN oldPen=(HPEN)SelectObject(hDC, hPen);
	 HBRUSH oldBrush=(HBRUSH)SelectObject(hDC, hBrush);

	 Rectangle(hDC, MOVESTARTX + UNITWIDTH*food.x, MOVESTARTY+UNITWIDTH*food.y, 
		 MOVESTARTX+UNITWIDTH + UNITWIDTH*food.x,  MOVESTARTY+UNITWIDTH*food.y+UNITWIDTH);//���߳Ե�ʳ�����food������ɣ�
	 
	 hPen=(HPEN)SelectObject(hDC,oldPen);
	 hBrush=(HBRUSH)SelectObject(hDC,oldBrush);
	 DeleteObject(hPen);
	 DeleteObject(hBrush);

	 ReleaseDC(hwnd,hDC);
}

void Snake::UnDrawSnake()/*���»�һ���հ׾��ο򣬸���ԭ���Ļ���, �����µ÷ֺ͹���*/
{
	 //HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	 HDC hDC=GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	 HPEN hPen; //����
	 HBRUSH hBrush;
 
	 hPen=CreatePen(PS_SOLID, 2, 0x0000ff);
	 hBrush=CreateSolidBrush(RGB(255,255,255)); 
	 HPEN oldPen=(HPEN)SelectObject(hDC, hPen);
	 HBRUSH oldBrush=(HBRUSH)SelectObject(hDC, hBrush);
	 Rectangle(hDC, MOVESTARTX, MOVESTARTY, MOVESTARTX+MOVEWIDTH,MOVESTARTY+MOVEHEITH);

	 hPen=(HPEN)SelectObject(hDC,oldPen);
	 hBrush=(HBRUSH)SelectObject(hDC,oldBrush);
	 DeleteObject(hPen);
	 DeleteObject(hBrush);

	 int X=0;	
	 int Y=0;			
	 SIZE size;
	 PutOutText(" ��  ",X,Y,RGB(0,255,0));

	 GetTextExtentPoint32(hDC," ��  ",strlen(" ��  "),&size);//��ȡ�ַ����Ŀ��
	 X=X+size.cx;
	 char tochar[10];
	 itoa(level,tochar,10);
	 PutOutText(tochar,X,Y,RGB(255,255,0));

	 GetTextExtentPoint32(hDC,tochar,strlen(tochar),&size);//��ȡ�ַ����Ŀ��
	 X=X+size.cx;
	 PutOutText(" ��",X,Y,RGB(0,255,0));

	 if (food.special == 1) 
	 {
		Y=2*FONTHEITH;
		X=0;
		PutOutText("ÿ����ֵ:",X,Y,RGB(255,0,0));
		GetTextExtentPoint32(hDC,"ÿ����ֵ:  ",strlen("ÿ����ֵ:  "),&size);//��ȡ�ַ����Ŀ��
		X=X+size.cx;
		itoa(2*per,tochar,10);
		PutOutText(tochar,X,Y,RGB(255,255,0));

		X=0;
		Y=4*FONTHEITH;
		PutOutText("���ĳ��֣�",X,Y,RGB(0,255,0));
	 }
	 else 
	 {
		Y=2*FONTHEITH;
		X=0;
		PutOutText("ÿ����ֵ:",X,Y,RGB(0,255,0));
		GetTextExtentPoint32(hDC,"ÿ����ֵ:  ",strlen("ÿ����ֵ:  "),&size);//��ȡ�ַ����Ŀ��
		X=X+size.cx;
		itoa(per,tochar,10);
		PutOutText(tochar,X,Y,RGB(255,255,0));
		X=0;
		Y=4*FONTHEITH;
		PutOutText("              ",X,Y,RGB(0,255,0));
	 }
	 X=0;
	 Y=6*FONTHEITH;
	 PutOutText("�÷�:",X,Y,RGB(0,255,0));
	 GetTextExtentPoint32(hDC,"�÷�:  ",strlen("�÷�:  "),&size);//��ȡ�ַ����Ŀ��
     X=X+size.cx;
	 itoa(score,tochar,10);
	 PutOutText(tochar,X,Y,RGB(255,255,0));

	 ReleaseDC(hwnd,hDC);
}

void Snake::DrawSnake()/*����*/
{
	 //HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	 HDC hDC=GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	 HPEN hPen; //����
	 HBRUSH hBrush;
 
	 hPen=CreatePen(PS_SOLID, 0, 0xffffff);
	 hBrush=CreateSolidBrush(RGB(0,0,0)); 
	 HPEN oldPen=(HPEN)SelectObject(hDC, hPen);
	 HBRUSH oldBrush=(HBRUSH)SelectObject(hDC, hBrush);

	 for (int i=0; i<curr; i++)
		 Rectangle(hDC, MOVESTARTX + UNITWIDTH*data[i].x, MOVESTARTY+UNITWIDTH*data[i].y, 
				MOVESTARTX+ UNITWIDTH*(data[i].x+1),  MOVESTARTY+UNITWIDTH*(data[i].y+1));
		 //Rectangle(hDC, 100 + 10*data[i].x, 10*data[i].y, 110 + 10*data[i].x, 10 + 10*data[i].y);
	 
	 hPen=(HPEN)SelectObject(hDC,oldPen);
	 hBrush=(HBRUSH)SelectObject(hDC,oldBrush);
	 DeleteObject(hPen);
	 DeleteObject(hBrush);

	 ReleaseDC(hwnd,hDC);
}

void Snake::TurnSnake() //���巴��
{
	SnakeNode t;
	for (int i = 0; i < curr/2; i++) {
		t = data[i]; 
		data[i] = data[curr-i-1]; 
		data[curr-i-1] = t;
	}
}

bool Snake::RandomFood() //�������һ���������Ʒλ��
{
	bool flag;
	int newx, newy;
	srand(time(NULL));
	int t = 0;
	do {
		flag = false;
		newx = rand()%20;
		newy = rand()%30;
		for (int i = 0; i < curr; i++)
			if (newx == data[i].x && newy == data[i].y)
			{
				flag = true;
				break;
			}

		t++;
		if (t > 10000) break;
	} while (flag);

	food.x = newx;
	food.y = newy;
	if (tot % 5 == 4) {
		food.special = 1;	//���ɺ���
		per;
	}
	else 
		food.special = 0;	//������ͨ��

	if (t <= 10000) 
		return true; 
	else				//�������Ѿ�û�е���԰ڷ�ʳ����
		return false;
}

void Snake::EatFood() {
	if (data[0].x == food.x && data[0].y == food.y)
	{
		curr++;		//������ľ���Ԥ���ŵ����һ��
		tot++;
		if (tot % 5 == 0)
		{
			level++;
			speed = speed; //�����ٶȺ�����ʵ�����ٶȣ��Ƕ�ʱ���������Ӧ�ñ�С
			score += 2*per++;		//����ĺ�㽱����
		} else
			score += per;

		if (!RandomFood()) {	// GameOver �Ҳ����ɷ���Ʒ�ĵ�
			data[0].x = -1;
			data[0].y = -1;
		}

	}
}

void Snake::MoveSnake()
{
	int i = 0;

	data[curr] = data[curr-1];	//Ϊ����Ʒ����״̬

	for (i=curr-1;i>=1;i--) {	//�ر�ע�⣺Ҫ���򣬼���˺þõ�
		data[i] = data[i-1];
	}

	data[0].x+=dx[direct];  //0����ͷ
	data[0].y+=dy[direct];
	
}

bool Snake::GameOver() {
	bool over = false;

	if (data[0].x < 0 || data[0].y < 0 || data[0].x >= 20 || data[0].y >= 30) //��߽��ཻ
		over=true;
	for (int i = 1; i < curr; i++)  //�������ཻ
		if (data[0].x == data[i].x && data[0].y == data[i].y) 
		{
			over=true;
			break;
		}

	if (over)  
	{
		char message[60];
		sprintf(message,"��ϲ������ %d ��,\n��ĵ÷�Ϊ %d\0",level,score);
		MessageBox(NULL,message,"GameOver",MB_OK);
		nGameState=0;
		return true;
	} 
	else
		return false;
}

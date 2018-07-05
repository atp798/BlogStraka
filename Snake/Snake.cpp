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

int sx;				// 记录开始的位置
int sy;				
int d;				// 该变量用于控制蛇身不能后退

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void PutOutText(char* lpsz,int X,int Y,unsigned long fontcolor){
	HDC hdc;
	HFONT hF_black;	//定义两种字体句柄
	TEXTMETRIC tm;			//定义一个TEXTMETRIC结构，用以记录字体信息

	//HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	hdc=GetDC((HWND)hwnd);//通过窗口句柄得到该窗口的设备场境句柄
	hF_black=CreateFont		//创建自定义字体
			(
				FONTHEITH,				//字体的高度
				0,				//由系统根据高宽比选取字体最佳宽度值
				0,				//文本的倾斜度为0，表示水平
				0,				//字体的倾斜度为0
				FW_NORMAL,		//字体的粗度，FW_HEAVY为最粗
				0,				//非斜体字
				0,				//无下划线
				0,				//无删除线
				GB2312_CHARSET,	//表示所用的字符集为ANSI_CHARSET
				OUT_DEFAULT_PRECIS,	//输出精度为缺省精度
				CLIP_DEFAULT_PRECIS,	//剪裁精度为缺省精度
				DEFAULT_QUALITY,		//输出质量为缺省值
				DEFAULT_PITCH|FF_DONTCARE,//字间距和字体系列使用缺省值
				"粗体字"				//字体名称
				);
		SetTextColor(hdc,fontcolor);	//设置文本颜色为绿色
		SetBkColor(hdc,RGB(0,0,0));
		SelectObject(hdc,hF_black);		//将自定义字体选入设备环境
		GetTextMetrics(hdc,&tm);		//获取字体的信息，并写入tm结构中

		TextOut(hdc,X,Y,lpsz,strlen(lpsz));	//使用当前字体输出文本
		Y=Y+tm.tmHeight+tm.tmExternalLeading;//计算换行时下一行文本的输出坐标
	
		ReleaseDC(hwnd,hdc);
		DeleteObject(hF_black);		//删除自定义字体句柄
}

Snake::Snake()
{
	direct = 1;		//向右
	nGameState=1;	
	score=0;

	tot=0;			//吃掉的物品数
	level=1;		//当前等级
	per=1;			//每个物品的得分
	speed=200;		//初始速度
	d=0;
	curr=2;			//当前蛇长度
	sx=sy=4;		//初始坐标
	
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
	 //HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	 HDC hDC=GetDC((HWND)hwnd);//通过窗口句柄得到该窗口的设备场境句柄
	 HPEN hPen; //画笔
	 HBRUSH hBrush;
 
	 hPen=CreatePen(PS_SOLID, 0, 0xff00ff);
	 if (food.special == 0) 
		 hBrush=CreateSolidBrush(0x000000); 
	 else
		 hBrush=CreateSolidBrush(0x0000ff);
	 
	 HPEN oldPen=(HPEN)SelectObject(hDC, hPen);
	 HBRUSH oldBrush=(HBRUSH)SelectObject(hDC, hBrush);

	 Rectangle(hDC, MOVESTARTX + UNITWIDTH*food.x, MOVESTARTY+UNITWIDTH*food.y, 
		 MOVESTARTX+UNITWIDTH + UNITWIDTH*food.x,  MOVESTARTY+UNITWIDTH*food.y+UNITWIDTH);//在蛇吃到食物后，让food随机生成？
	 
	 hPen=(HPEN)SelectObject(hDC,oldPen);
	 hBrush=(HBRUSH)SelectObject(hDC,oldBrush);
	 DeleteObject(hPen);
	 DeleteObject(hBrush);

	 ReleaseDC(hwnd,hDC);
}

void Snake::UnDrawSnake()/*重新画一个空白矩形框，覆盖原来的画面, 并更新得分和关数*/
{
	 //HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	 HDC hDC=GetDC((HWND)hwnd);//通过窗口句柄得到该窗口的设备场境句柄
	 HPEN hPen; //画笔
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
	 PutOutText(" 第  ",X,Y,RGB(0,255,0));

	 GetTextExtentPoint32(hDC," 第  ",strlen(" 第  "),&size);//获取字符串的宽度
	 X=X+size.cx;
	 char tochar[10];
	 itoa(level,tochar,10);
	 PutOutText(tochar,X,Y,RGB(255,255,0));

	 GetTextExtentPoint32(hDC,tochar,strlen(tochar),&size);//获取字符串的宽度
	 X=X+size.cx;
	 PutOutText(" 关",X,Y,RGB(0,255,0));

	 if (food.special == 1) 
	 {
		Y=2*FONTHEITH;
		X=0;
		PutOutText("每个分值:",X,Y,RGB(255,0,0));
		GetTextExtentPoint32(hDC,"每个分值:  ",strlen("每个分值:  "),&size);//获取字符串的宽度
		X=X+size.cx;
		itoa(2*per,tochar,10);
		PutOutText(tochar,X,Y,RGB(255,255,0));

		X=0;
		Y=4*FONTHEITH;
		PutOutText("红心出现！",X,Y,RGB(0,255,0));
	 }
	 else 
	 {
		Y=2*FONTHEITH;
		X=0;
		PutOutText("每个分值:",X,Y,RGB(0,255,0));
		GetTextExtentPoint32(hDC,"每个分值:  ",strlen("每个分值:  "),&size);//获取字符串的宽度
		X=X+size.cx;
		itoa(per,tochar,10);
		PutOutText(tochar,X,Y,RGB(255,255,0));
		X=0;
		Y=4*FONTHEITH;
		PutOutText("              ",X,Y,RGB(0,255,0));
	 }
	 X=0;
	 Y=6*FONTHEITH;
	 PutOutText("得分:",X,Y,RGB(0,255,0));
	 GetTextExtentPoint32(hDC,"得分:  ",strlen("得分:  "),&size);//获取字符串的宽度
     X=X+size.cx;
	 itoa(score,tochar,10);
	 PutOutText(tochar,X,Y,RGB(255,255,0));

	 ReleaseDC(hwnd,hDC);
}

void Snake::DrawSnake()/*画蛇*/
{
	 //HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	 HDC hDC=GetDC((HWND)hwnd);//通过窗口句柄得到该窗口的设备场境句柄
	 HPEN hPen; //画笔
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

void Snake::TurnSnake() //整体反向
{
	SnakeNode t;
	for (int i = 0; i < curr/2; i++) {
		t = data[i]; 
		data[i] = data[curr-i-1]; 
		data[curr-i-1] = t;
	}
}

bool Snake::RandomFood() //随机生成一个合理的物品位置
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
		food.special = 1;	//生成红心
		per;
	}
	else 
		food.special = 0;	//生成普通点

	if (t <= 10000) 
		return true; 
	else				//区域中已经没有点可以摆放食物了
		return false;
}

void Snake::EatFood() {
	if (data[0].x == food.x && data[0].y == food.y)
	{
		curr++;		//多出来的就是预留着的最后一个
		tot++;
		if (tot % 5 == 0)
		{
			level++;
			speed = speed; //这里速度含义其实不是速度，是定时间隔，所以应该变小
			score += 2*per++;		//特殊的红点奖励分
		} else
			score += per;

		if (!RandomFood()) {	// GameOver 找不到可放物品的点
			data[0].x = -1;
			data[0].y = -1;
		}

	}
}

void Snake::MoveSnake()
{
	int i = 0;

	data[curr] = data[curr-1];	//为吃物品保存状态

	for (i=curr-1;i>=1;i--) {	//特别注意：要倒序，检查了好久的
		data[i] = data[i-1];
	}

	data[0].x+=dx[direct];  //0是蛇头
	data[0].y+=dy[direct];
	
}

bool Snake::GameOver() {
	bool over = false;

	if (data[0].x < 0 || data[0].y < 0 || data[0].x >= 20 || data[0].y >= 30) //与边界相交
		over=true;
	for (int i = 1; i < curr; i++)  //与自身相交
		if (data[0].x == data[i].x && data[0].y == data[i].y) 
		{
			over=true;
			break;
		}

	if (over)  
	{
		char message[60];
		sprintf(message,"恭喜闯到第 %d 关,\n你的得分为 %d\0",level,score);
		MessageBox(NULL,message,"GameOver",MB_OK);
		nGameState=0;
		return true;
	} 
	else
		return false;
}

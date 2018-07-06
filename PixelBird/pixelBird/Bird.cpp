#include "StdAfx.h"
#include "Bird.h"

Bird::Bird(void)
{
	g=2.5;
	vx=0.0;
	vy=0.0;
	x=CONSOLEX /2.0;
	y=CONSOLEY/2.0;
	upa = -12.0;
	memset(birddot,0,sizeof(birddot));
	birddot[0][0][1]=1;
	birddot[0][1][2]=1;
	birddot[0][2][0]=1;birddot[0][2][1]=1;birddot[0][2][2]=1;birddot[0][2][3]=1;birddot[0][2][4]=1;
	birddot[0][3][2]=1;
	birddot[0][4][1]=1;
	birddot[1][1][1]=1;birddot[1][1][2]=1;
	birddot[1][2][0]=1;birddot[1][2][1]=1;birddot[1][2][2]=1;birddot[1][2][3]=1;birddot[1][2][4]=1;
	birddot[1][3][1]=1;birddot[1][3][2]=1;
	birdstate = 0;
}


Bird::~Bird(void)
{
}

void Bird::jump(){
	vy = upa;
}

void Bird::move(){
	vy += g;
	y += vy;
}

void Bird::drawBD(){
	HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	HDC hDC = GetDC((HWND)hwnd);//通过窗口句柄得到该窗口的设备场境句柄
	HPEN hPen; //画笔
	HBRUSH hBrush;

	/*
	hPen = CreatePen(PS_SOLID, 0, 0xffffff);
	hBrush = CreateSolidBrush(RGB(0,0,0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	Rectangle(hDC,0,0,CONSOLEX,CONSOLEY);
	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
	*/

	hPen = CreatePen(PS_SOLID, 0, 0x000000);
	hBrush = CreateSolidBrush(RGB(0,0, 255));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	int i,j;
	for (i = 0; i < 5; i++){
		for(j = 0; j < 5; j++){
			if(birddot[birdstate][i][j] > 0){
				Rectangle(hDC, x+5*j,y+5*i,x+5*j+5,y+5*i+5);
			}
		}
	}

	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(oldPen);
	DeleteObject(oldBrush);
	ReleaseDC(hwnd, hDC);
}

void Bird::changeBdSt(){
	birdstate = 1-birdstate;
}
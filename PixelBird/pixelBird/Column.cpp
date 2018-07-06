#include "StdAfx.h"
#include "Column.h"
#include "windows.h"
#include "time.h"

Column::Column(void)
{
	srand(time(NULL));
	vx = -4.0;
	x = CONSOLEX;
	height = rand()%28*10;
	gapheight = rand()/RAND_MAX*(GAPMAX-GAPMIN)+GAPMIN;
	width = 25;
	//do{
		//height = rand()/RAND_MAX*;
	//}while(height>400-GAPMAX);
}

Column::~Column(void)
{
}

void Column::move(){
	x += vx;
}

void Column::draw(){
		HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	HDC hDC = GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	HPEN hPen; //����
	HBRUSH hBrush;
	
	hPen = CreatePen(PS_SOLID, 0, 0xff0000);
	hBrush = CreateSolidBrush(RGB(0, 255, 0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	int tmp = (x+width > CONSOLEX)?CONSOLEX:x+width;
	int tmpleft = x>0 ? x:0;

	Rectangle(hDC,x,0,tmp,height);
	Rectangle(hDC,x,height+gapheight,tmp,CONSOLEY);

	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(oldPen);
	DeleteObject(oldBrush);
	ReleaseDC(hwnd, hDC);

}

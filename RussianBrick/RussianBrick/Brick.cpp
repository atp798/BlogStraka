#include "stdafx.h"
#include"Brick.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <time.h>
#include <windows.h>

#define FONTHEITH 15 //remark ,the font is not all limited by this define, 15 is the default

extern HWND hwnd;

//the seven type brick's locate
int typeBK[7][4][2] = {
		{ { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } },
		{ { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } },
		{ { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 } },
		{ { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
		{ { -1, 1 }, { 0, 0 }, { 0, 1 }, { 1, 0 } },
		{ { -2, 0 }, { -1, 0 }, { 0, 0 }, { 0, 1 } },
		{ { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } }
};
//the min/max x/y of the brick
int typeBKxy[7][4] = {
		{ 0, 1, 0, 1 },
		{ -1, 2, 0, 0 },
		{ 0, 2, 0, 1 },
		{ -1, 1, 0, 1 },
		{ -1, 1, 0, 1 },
		{ -2, 0, 0, 1 },
		{ -1, 1, 0, 1 }
};

Brick::Brick(){
	srand(time(NULL));
	
	memset(gameArea, 0, sizeof(gameArea));
	score = 0;
	speed = 500;

	nextType = rand() % 7;
	firstrandBK();
	judgeScore();
}

Brick::~Brick(){

}

//output certain text to the certain places
void PutOutText(char* lpsz, int X, int Y, unsigned long fontcolor){
	HDC hdc;
	HFONT hF_black;	//��������������
	TEXTMETRIC tm;			//����һ��TEXTMETRIC�ṹ�����Լ�¼������Ϣ

	//HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	hdc = GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	hF_black = CreateFont		//�����Զ�������
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
		DEFAULT_PITCH | FF_DONTCARE,//�ּ�������ϵ��ʹ��ȱʡֵ
		(LPCWSTR)"������"				//��������
		);
	SetTextColor(hdc, fontcolor);	//�����ı���ɫΪ��ɫ
	SetBkColor(hdc, RGB(0, 0, 0));
	SelectObject(hdc, hF_black);		//���Զ�������ѡ���豸����
	GetTextMetrics(hdc, &tm);		//��ȡ�������Ϣ����д��tm�ṹ��

	TextOutA(hdc, X, Y,lpsz, strlen(lpsz));	//ʹ�õ�ǰ��������ı�

	ReleaseDC(hwnd, hdc);
	DeleteObject(hF_black);		//ɾ���Զ���������
}

void Brick::firstrandBK(){
	int randType = rand() % 7;
	memcpy(activeBK.unit, typeBK[randType], sizeof(activeBK.unit));
	activeBK.minX = typeBKxy[randType][0];
	activeBK.maxX = typeBKxy[randType][1];
	activeBK.minY = typeBKxy[randType][2];
	activeBK.maxY = typeBKxy[randType][3];
	nextType = rand() % 7;
	nextDirect = rand() % 4;
	rotateBK(rand() % 4, 0);
	do{
		activeBK.locaX = rand() % AREAWIDTH;
		activeBK.locaY = -activeBK.maxY;

	} while ((activeBK.minX + activeBK.locaX) < 0 || (activeBK.maxX + activeBK.locaX) > AREAWIDTH - 1);
	drawNextBk();
}

void Brick::randBK(){
	memcpy(activeBK.unit, typeBK[nextType], sizeof(activeBK.unit));
	activeBK.minX = typeBKxy[nextType][0];
	activeBK.maxX = typeBKxy[nextType][1];
	activeBK.minY = typeBKxy[nextType][2];
	activeBK.maxY = typeBKxy[nextType][3];
	nextType = rand() % 7;
	rotateBK(nextDirect,0);
	nextDirect = rand() % 4;
	do{
		activeBK.locaX = rand() % AREAWIDTH;
		activeBK.locaY = -activeBK.maxY-1;
		
	} while ((activeBK.minX + activeBK.locaX) < 0 || (activeBK.maxX + activeBK.locaX) > AREAWIDTH - 1);
	drawNextBk();
}

void Brick::formNext(){
	memcpy(nextBK.unit, typeBK[nextType], sizeof(nextBK.unit));
	nextBK.minX = typeBKxy[nextType][0];
	nextBK.maxX = typeBKxy[nextType][1];
	nextBK.minY = typeBKxy[nextType][2];
	nextBK.maxY = typeBKxy[nextType][3];
	rotateBK(nextDirect, 0);
	nextBK.locaX = (BRICKWIDTH - nextBK.minX - nextBK.maxX) / 2;
	nextBK.locaY = (BRICKHEITH - nextBK.minY - nextBK.maxY) / 2;
}

void Brick::drawNextBk(){
	formNext();
	HDC hDC = GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	HPEN hPen, hPenBK; //����
	HBRUSH hBrush, hBrushBK;

	hPen = CreatePen(PS_SOLID, 0, 0x0000ff);
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hPenBK = CreatePen(PS_SOLID, 0, 0xffffff);
	hBrushBK = CreateSolidBrush(RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, BRICKSTARTX,BRICKSTARTY,BRICKSTARTX + BRICKWIDTH*BKUWIDTH, BRICKSTARTY + BRICKHEITH*BKUWIDTH);

	SelectObject(hDC, hPenBK);
	SelectObject(hDC, hBrushBK);
	int i, j;
	for (i = 0; i < 4; i++){
		Rectangle(hDC, BRICKSTARTX + (nextBK.locaX + nextBK.unit[i][0]) * BKUWIDTH,
			BRICKSTARTY + (nextBK.locaY + nextBK.unit[i][1]) * BKUWIDTH, 
			BRICKSTARTX + (nextBK.locaX + nextBK.unit[i][0] + 1) * BKUWIDTH,
			BRICKSTARTY + (nextBK.locaY + nextBK.unit[i][1] + 1) * BKUWIDTH
			);
	}
	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hPenBK);
	DeleteObject(hBrushBK);
	ReleaseDC(hwnd, hDC);

}

bool Brick::moveBK(int direct){
	int fmove=1;
	switch (direct){
	case 0:
		activeBK.locaX += 1;
		if (!judgeBK()){
			activeBK.locaX -= 1;
			fmove = 0;
		}
		break;
	case 1:
		activeBK.locaY += 1; 
		if (!judgeBK()){
			activeBK.locaY -= 1;
			fmove = 0;
		}
		break;
	case 2:
		activeBK.locaX -= 1;
		if (!judgeBK()){
			activeBK.locaX += 1;
			fmove = 0;
		}
		break; 
	case 3:
		activeBK.locaY -= 1;
		if (!judgeBK()){
			activeBK.locaY += 1;
			fmove = 0;
		}
		break;
	case 4:
		if (!rotateBK(1,1)){
			fmove = 0;
		}
		break;
	default:
		break;
	}
	return fmove;
}

bool Brick::rotateBK(int deg,int ifJudge){
	int i,j;
	if (deg < 0)
		deg += 4;
	for (i = 0; i < deg; i++){
		int tmpx;
		tmpx = activeBK.minX;
		activeBK.minX = -activeBK.maxY;
		activeBK.maxY = activeBK.maxX;
		activeBK.maxX = -activeBK.minY;
		activeBK.minY = tmpx;
		for (j = 0; j < 4; j++){
			tmpx = activeBK.unit[j][0];
			activeBK.unit[j][0] = -activeBK.unit[j][1];
			activeBK.unit[j][1] = tmpx;
		}
	}
	if (ifJudge){
		if (!judgeBK()){
			rotateBK(-deg,0);
			return false;
		}
	}
	return true;
	
}

bool Brick::judgeBK(){
	if ((activeBK.minX + activeBK.locaX) < 0 || (activeBK.maxX + activeBK.locaX) > AREAWIDTH - 1)
		return false;
	if ((activeBK.maxY + activeBK.locaY)> AREAHEITH - 1)
		return false;
	int i;
	for (i = 0; i < 4; i++){
		int tmpx, tmpy;
		tmpx = activeBK.unit[i][0] + activeBK.locaX;
		tmpy = activeBK.unit[i][1] + activeBK.locaY;
		if (tmpx<0 || tmpx>AREAWIDTH - 1 || tmpy>AREAHEITH - 1)
			return false;
		if (tmpy>=0)
			if (gameArea[tmpy][tmpx]>0)
				return false;
	}
	return true;
}

void Brick::updateArea(){
	int i;
	for (i = 0; i < 4; i++){
		int tmpx = activeBK.unit[i][0] + activeBK.locaX;
		int tmpy = activeBK.unit[i][1] + activeBK.locaY;
		if (tmpx>=0&&tmpx<AREAWIDTH&&tmpy>=0&&tmpy<AREAHEITH)
			gameArea[tmpy][tmpx] = 1;
	}
}

void Brick::drawBK(){
	//HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	HDC hDC = GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	HPEN hPen; //����
	HBRUSH hBrush;

	hPen = CreatePen(PS_SOLID, 0, 0xffffff);
	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	int i;
	for (i = 0; i < 4; i++){
		if (activeBK.unit[i][1]+activeBK.locaY>=0)
			Rectangle(hDC,  (activeBK.unit[i][0] + activeBK.locaX)*UNITWIDTH + AREASTARTX,
							(activeBK.unit[i][1] + activeBK.locaY)*UNITWIDTH + AREASTARTY,
							(activeBK.unit[i][0] + activeBK.locaX+1)*UNITWIDTH + AREASTARTX,
							(activeBK.unit[i][1] + activeBK.locaY+1)*UNITWIDTH + AREASTARTY);
	}

	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	ReleaseDC(hwnd, hDC);
}

void Brick::drawArea(){
	//HWND hwnd=GetForegroundWindow(); //if get by this way, it will draw on other window which forcus be changed to
	HDC hDC = GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	HPEN hPen,hPenBK; //����
	HBRUSH hBrush,hBrushBK;

	hPen = CreatePen(PS_SOLID, 0, 0x0000ff);
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hPenBK = CreatePen(PS_SOLID, 0, 0xffffff);
	hBrushBK = CreateSolidBrush(RGB(0,0,0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, AREASTARTX, AREASTARTY, AREASTARTX + AREAWIDTH*UNITWIDTH, AREASTARTY + AREAHEITH*UNITWIDTH);

	SelectObject(hDC, hPenBK);
	SelectObject(hDC, hBrushBK);
	int i, j;
	for (i = 0; i < AREAHEITH; i++){
		for (j = 0; j < AREAWIDTH; j++){
			if (gameArea[i][j] > 0)
				Rectangle( hDC,  j*UNITWIDTH + AREASTARTX,
							     i*UNITWIDTH + AREASTARTY,
								(j+1)*UNITWIDTH + AREASTARTX,
								(i+1)*UNITWIDTH + AREASTARTY
								);
		}
	}

	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hPenBK);
	DeleteObject(hBrushBK);
	ReleaseDC(hwnd, hDC);
}

bool Brick::judgeScore(){
	int i, j;
	int fRemove = 0;
	for (i = 0; i < AREAHEITH; i++){
		int rowctn=0;
		for (j = 0; j < AREAWIDTH; j++){
			if (gameArea[i][j] > 0)
				rowctn++;
		}
		if (rowctn == AREAWIDTH){
			removeRow(i);
			score++;
			fRemove++;
		}
	}
	HDC hDC = GetDC((HWND)hwnd);
	int X = 0;
	int Y = 45;
	SIZE size;
	GetTextExtentPoint32A(hDC, "Your score : ", strlen("Your score : "), &size);//��ȡ�ַ����Ŀ��
	X = X + size.cx;
	char tochar[10];
	tochar[0] = ' ';
	_itoa_s(score, tochar, 10);
	PutOutText(tochar, X + 5, Y, RGB(255, 255, 0));

	return fRemove>0?true:false;
}

void Brick::removeRow(int row){
	int i, j;
	for (i = row-1; i > 0; i--){
		for (j = 0; j < AREAWIDTH; j++){
			gameArea[i + 1][j] = gameArea[i][j];
		}
	}
}

bool Brick::gameOver(){
	int i;
	for (i = 0; i < AREAWIDTH; i++){
		if (gameArea[0][i] > 0){
			MessageBoxA(hwnd, "Game Over", "AOu", MB_OK);
			return true;
		}
	}
	return false;
}

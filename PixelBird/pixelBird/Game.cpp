#include "stdafx.h"
#include "Game.h"

Bird* pbird; 
Column* pcol[5];

int gamestate = 0;
int ifStart = 0;
int ifPause = 0;

int colctn =0;
int bdstctn=0;
int score =0;
int iscoretmp = 0;
int itmp = 0;
HWND hwndtxt = NULL;

bool ifGameover(){
	if((*pbird).y<=0||(*pbird).y>=(CONSOLEY-25))
		return true;
	int i;
	for(i=0;i<colctn;i++){
		if(pbird->x>=pcol[i]->x-25&&pbird->x<=pcol[i]->x+25){
			iscoretmp = 1;
			itmp = i;
			if(pbird->y<pcol[i]->height||(pbird->y>(pcol[i]->height+pcol[i]->gapheight-25)))
				return true;
			else 
				break;
		}
		else{
			if(iscoretmp==1&&i==itmp){score++; iscoretmp = 0;}
			continue;
		}
	}
	return false;
}

void printResult(){
		HDC hDC = GetDC((HWND)hwndtxt);
		int X = 0;
		int Y = CONSOLEY;
		SIZE size;
		PutOutText("Your score :       ", X, Y, RGB(0, 255, 255));

		GetTextExtentPoint32A(hDC,"Your score :    ", strlen("Your score :    "), &size);//��ȡ�ַ����Ŀ��
		X = X + size.cx;
		char tochar[10];
		_itoa_s(score, tochar, 10);
		PutOutText(tochar, X, Y, RGB(255, 255, 0));

		X = 0;
		Y = CONSOLEY + FONTHEITH;
		PutOutText("                                                                                 ", X, Y, RGB(0, 255, 255));
		PutOutText("Game state: ", X, Y, RGB(0, 255, 255));

		GetTextExtentPoint32A(hDC,"Game state: ",strlen("Game state: "),&size);
		X = X + size.cx;
		if(!ifStart)
			PutOutText("unStarted", X, Y, RGB(255, 255,0));
		else if(!ifPause)
			PutOutText("Running    ", X, Y, RGB(255, 255,0));
		else 
			PutOutText("Paused    ", X, Y, RGB(255, 255,0));
		ReleaseDC(hwndtxt, hDC);
}

void printInfo(){
		HDC hDC = GetDC((HWND)hwndtxt);
		int X = 0;
		int Y = CONSOLEY;
		SIZE size;
		PutOutText("Rules :  ", X, Y, RGB(0, 255, 255));

		X = 0;
		Y = CONSOLEY + FONTHEITH;
		PutOutText("Press 'f' to start games, Press 'p' to pause, Press ' ' or 'j' to make the bird jump", X, Y, RGB(0, 255, 255));
		ReleaseDC(hwndtxt, hDC);
}

void movecolumn(){
	int i;
	switch(colctn){
	case 1:

		//break;
	case 2:
		//break;
	case 3:
		//break;
	case 4:
		for(i=0;i<colctn;i++){
			pcol[i]->move();
			pcol[i]->draw();
		}
		if((pcol[colctn-1]->x)<(CONSOLEX-160)){
			pcol[(++colctn)-1] = new Column;
		}

		break;
	case 5:
		for(i=0;i<5;i++){
			pcol[i]->move();
			pcol[i]->draw();
			if((pcol[i]->x)<=-25){
				delete pcol[i];
				pcol[i] = new Column;
			}
		}
		break;
	}
}

void drawBG(){
	HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	HDC hDC = GetDC((HWND)hwnd);//ͨ�����ھ���õ��ô��ڵ��豸�������
	HPEN hPen; //����
	HBRUSH hBrush;
	
	hPen = CreatePen(PS_SOLID, 0, 0xffffff);
	hBrush = CreateSolidBrush(RGB(0,0,0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	
	Rectangle(hDC,0,0,CONSOLEX,CONSOLEY);
	
	hPen = (HPEN)SelectObject(hDC, oldPen);
	hBrush = (HBRUSH)SelectObject(hDC, oldBrush);
		
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(oldPen);
	DeleteObject(oldBrush);
	ReleaseDC(hwnd, hDC);

}

void PutOutText(char* lpsz, int X, int Y, unsigned long fontcolor){
	HDC hdc;
	HFONT hF_black;	//��������������
	TEXTMETRIC tm;			//����һ��TEXTMETRIC�ṹ�����Լ�¼������Ϣ

	//HWND hwnd=GetForegroundWindow(); //�ҵ��������д��ڵľ��
	hdc = GetDC((HWND)hwndtxt);//ͨ�����ھ���õ��ô��ڵ��豸�������
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
	Y = Y + tm.tmHeight + tm.tmExternalLeading;//���㻻��ʱ��һ���ı����������

	ReleaseDC(hwndtxt, hdc);
	DeleteObject(hF_black);		//ɾ���Զ���������
}

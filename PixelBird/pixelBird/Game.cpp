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

		GetTextExtentPoint32A(hDC,"Your score :    ", strlen("Your score :    "), &size);//获取字符串的宽度
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
	HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	HDC hDC = GetDC((HWND)hwnd);//通过窗口句柄得到该窗口的设备场境句柄
	HPEN hPen; //画笔
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
	HFONT hF_black;	//定义两种字体句柄
	TEXTMETRIC tm;			//定义一个TEXTMETRIC结构，用以记录字体信息

	//HWND hwnd=GetForegroundWindow(); //找到程序运行窗口的句柄
	hdc = GetDC((HWND)hwndtxt);//通过窗口句柄得到该窗口的设备场境句柄
	hF_black = CreateFont		//创建自定义字体
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
		DEFAULT_PITCH | FF_DONTCARE,//字间距和字体系列使用缺省值
		(LPCWSTR)"粗体字"				//字体名称
		);
	SetTextColor(hdc, fontcolor);	//设置文本颜色为绿色
	SetBkColor(hdc, RGB(0, 0, 0));
	SelectObject(hdc, hF_black);		//将自定义字体选入设备环境
	GetTextMetrics(hdc, &tm);		//获取字体的信息，并写入tm结构中

	TextOutA(hdc, X, Y,lpsz, strlen(lpsz));	//使用当前字体输出文本
	Y = Y + tm.tmHeight + tm.tmExternalLeading;//计算换行时下一行文本的输出坐标

	ReleaseDC(hwndtxt, hdc);
	DeleteObject(hF_black);		//删除自定义字体句柄
}

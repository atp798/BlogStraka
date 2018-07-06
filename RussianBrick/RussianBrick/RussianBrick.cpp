// RussianBrick.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RussianBrick.h"
#include "Brick.h"
#include <cstdio>

#define MAX_LOADSTRING 100
#define ID_TIMER 1
#define ID_TIMEOVER 2

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HWND hwnd;//handle of the console, used to draw 
int ifStart = 0; //flag indicate whether the game has started
int ifPause = 0; //flag of the game pausing
Brick * brik;    //the instance of the brick class

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RUSSIANBRICK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RUSSIANBRICK));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= 0;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUSSIANBRICK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //窗口背景为黑色
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hwnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	char lpsz_0[] = "Operation Rules:";
	char lpsz_1[] = "press 'f' to start, press 'q'to quit, press 'p'or space to pause\0";
	char lpsz_2[] = "up--W down--S left--A right--D,or use the direction key,counterclockwise--Y,clockwise--U,  游戏中不区分大小写\0";
	char lpsz_3[] = "rules:\0";
	char lpsz_4[] = "1.control the way of the brick falls down\0";
	char lpsz_5[] = "2.a line will disappear when it has been filled and the score will increase\0";
	char lpsz_6[] = "3.as the score goes high, the brick will fall down faster\0";
	char lpsz_7[] = "4.enjoy yourself\0";
	char lpsz_8[] = "5.once the fallen brick is stucked as soon as it comes out, the game is over\0";

	int X = 0, Y = 0;

	HFONT hF_black;	//定义两种字体句柄
	TEXTMETRIC tm;			//定义一个TEXTMETRIC结构，用以记录字体信息
	
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		hF_black = CreateFont		//创建自定义字体
			(
			15,				//字体的高度
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

		SetBkColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, hF_black);		//将自定义字体选入设备环境
		GetTextMetrics(hdc, &tm);		//获取字体的信息，并写入tm结构中

		SetTextColor(hdc, RGB(0, 255, 0));	//设置文本颜色为绿色
		TextOutW(hdc, X, Y, TEXT("Game status:"), strlen("Game status:"));

		SetTextColor(hdc, RGB(255, 0, 0));
		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		if (ifPause){
			TextOutA(hdc, X, Y, "Pause", strlen("Pause"));
		}
		else if (ifStart){
			TextOutA(hdc, X, Y, "Gaming", strlen("Gaming"));
		}
		else {
			TextOutA(hdc, X, Y, "unStart", strlen("unStart"));
		}

		Y = 45;
		SIZE size;
		SetTextColor(hdc, RGB(0, 255, 0));
		TextOutW(hdc, X, Y, TEXT("Your score : "), strlen("Your score : "));
		
		Y += tm.tmHeight;
		SetTextColor(hdc, RGB(255, 255, 0));
		TextOutA(hdc, X, Y,"The next brick:", strlen("The next brick:"));

		Y = AREASTARTY + AREAHEITH*UNITWIDTH;
		SetTextColor(hdc, RGB(0, 255,0));
		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_0, strlen(lpsz_0));	//使用当前字体输出文本

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_1, strlen(lpsz_1));

		Y = Y + tm.tmHeight + tm.tmExternalLeading;//计算换行时下一行文本的输出坐标
		TextOutA(hdc, X, Y,lpsz_2, strlen(lpsz_2));//使用当前字体输出文本

		//换行继续输出文本，计算新行的起始Y坐标位置
		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_3, strlen(lpsz_3));

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_4, strlen(lpsz_4));//use different way to output , W/A/TEXT

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_5, strlen(lpsz_5)); 	//输出文本

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_6, strlen(lpsz_6));//以当前字体输出文本

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_7, strlen(lpsz_7));//以当前字体输出文本

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_8, strlen(lpsz_8));//以当前字体输出文本
		DeleteObject(hF_black);		//删除自定义字体句柄
		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		if (wParam == ID_TIMER)
		{
			KillTimer(hWnd, ID_TIMER);
			if (!brik->moveBK(1)){
				brik->updateArea();
				brik->judgeScore();
				brik->randBK();
			}
			brik->drawArea();
			brik->drawBK();
			if (brik->gameOver()){
				ifStart = 0;
				delete brik;
			}
			else{
				SetTimer(hWnd, ID_TIMER, (*brik).speed, NULL);
			}
		}
		else if (wParam == ID_TIMEOVER){
			ifStart = 0;
			KillTimer(hWnd, ID_TIMEOVER);
			delete brik;
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'f':
		case 'F':
			if (!ifStart){
				ifStart = 1;
				brik = new Brick; 
				brik->drawArea();
				brik->drawBK();
				SetTimer(hwnd, ID_TIMER, (*brik).speed, NULL);
			}
			break;
		case 'p':
		case 'P':
			if (!ifStart)break;
			ifPause = ifPause == 0 ? 1 : 0;
			if (ifPause){
				KillTimer(hwnd, ID_TIMER);
			}
			else{
				SetTimer(hwnd, ID_TIMER, (*brik).speed, NULL);
			}
			RECT ret;
			GetWindowRect(hwnd, &ret);
			InvalidateRect(hwnd, &ret, true);
			UpdateWindow(hwnd);
			//SendMessage(hwnd, WM_PAINT, NULL, NULL);
			break;
		case 'w':
		case 'W':
			if (!ifStart || ifPause)break;
			brik->rotateBK(1,1);
			brik->drawArea();
			brik->drawBK();
			break;
		case 'd':
		case 'D':
			if (!ifStart || ifPause)break;
			brik->moveBK(0); 
			brik->drawArea();
			brik->drawBK();
			break;
		case 's':
		case 'S':
			KillTimer(hWnd, ID_TIMER);
			if (!ifStart || ifPause)break;
			if (!brik->moveBK(1)){
				brik->updateArea();
				brik->judgeScore();
				brik->randBK();
			}
			brik->drawArea();
			brik->drawBK();
			if (brik->gameOver()){
				ifStart = 0;
				delete brik;
			}
			else{
				SetTimer(hWnd, ID_TIMER, (*brik).speed, NULL);
			}
			break;
		case 'a':
		case 'A':
			if (!ifStart || ifPause)break;
			brik->moveBK(2);
			brik->drawArea();
			brik->drawBK();
			break;
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			break;
		case ' ':
			if (!ifStart)break;
			ifPause = ifPause == 0 ? 1 : 0;
			if (ifPause){
				KillTimer(hwnd, ID_TIMER);
			}
			else{
				SetTimer(hwnd, ID_TIMER, (*brik).speed, NULL);
			}
			break;
		case 'y':
		case 'Y':
			if (!ifStart || ifPause)break;
			brik->rotateBK(1,1);
			brik->drawArea();
			brik->drawBK();
			break;
		case 'u':
		case 'U':
			if (!ifStart || ifPause)break;
			brik->rotateBK(-1,1);
			brik->drawArea();
			brik->drawBK();
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if (!ifStart || ifPause)break;
			brik->rotateBK(1,1);
			brik->drawArea();
			brik->drawBK();
			break;
		case VK_RIGHT:
			if (!ifStart || ifPause)break;
			brik->moveBK(0);
			brik->drawArea();
			brik->drawBK();
			break;
		case VK_DOWN:
			KillTimer(hWnd, ID_TIMER);
			if (!ifStart || ifPause)break;
			if (!brik->moveBK(1)){
				brik->updateArea();
				brik->judgeScore();
				brik->randBK();
			}
			brik->drawArea();
			brik->drawBK();
			if (brik->gameOver()){
				ifStart = 0;
				delete brik;
			}
			else{
				SetTimer(hWnd, ID_TIMER, (*brik).speed, NULL);
			}
			break;
		case VK_LEFT:
			if (!ifStart || ifPause)break;
			brik->moveBK(2);
			brik->drawArea();
			brik->drawBK();
			break;

		default:
			break;
		}
		break;//这个地方很容易忽略，导致一有其他消息程序就EXIT;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

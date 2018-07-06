// pixelBird.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "pixelBird.h"

#define MAX_LOADSTRING 100
#define ID_TIMER 1
#define INTERVAL 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

extern int gamestate;
extern int ifStart;
extern int ifPause;

extern Bird* pbird; 
extern Column* pcol[5];
extern int colctn;
extern int bdstctn;
extern int score;
extern HWND hwndtxt;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PIXELBIRD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PIXELBIRD));

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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= 0; //CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIXELBIRD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_PIXELBIRD);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
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
      CW_USEDEFAULT, CW_USEDEFAULT, CONSOLEX, CONSOLEY+80 , NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hwndtxt = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
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

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		if (wParam == ID_TIMER)
		{
			KillTimer(hWnd, ID_TIMER);
			bdstctn++;
			bdstctn %=4;
			if(bdstctn ==3 ) pbird->changeBdSt();

			drawBG();
			pbird->move();
			movecolumn();
			if(ifGameover()){
				MessageBoxA(hwndtxt, "Game Over", "Ah..ou", MB_OK);
				ifStart = 0;
				delete pbird;
				int i;
				for(i=0;i<colctn;i++){
					delete pcol[i];
				}
			}
			else{
				pbird->drawBD();
				SetTimer(hWnd, ID_TIMER, INTERVAL, NULL);
			}
			printResult();
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'f':
		case 'F':
			if (!ifStart){
				ifStart = 1;
				pbird = new Bird; 
				drawBG();
				pbird->drawBD();
				pcol[0] = new Column;
				colctn = 1;
				bdstctn = 0;
				score = 0;
				SetTimer(hWnd, ID_TIMER,INTERVAL, NULL);
				printResult();
			}
			break;
		case 'p':
		case 'P':
			if (!ifStart)break;
			ifPause = ifPause == 0 ? 1 : 0;
			if (ifPause){
				KillTimer(hWnd, ID_TIMER);
			}
			else{
				SetTimer(hWnd, ID_TIMER, INTERVAL, NULL);
			}
			printResult();
			break;
		case ' ':
		case 'j':
		case 'J':
			if(ifStart&&!ifPause)
				pbird->jump();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


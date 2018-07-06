// RussianBrick.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "RussianBrick.h"
#include "Brick.h"
#include <cstdio>

#define MAX_LOADSTRING 100
#define ID_TIMER 1
#define ID_TIMEOVER 2

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

HWND hwnd;//handle of the console, used to draw 
int ifStart = 0; //flag indicate whether the game has started
int ifPause = 0; //flag of the game pausing
Brick * brik;    //the instance of the brick class

// �˴���ģ���а����ĺ�����ǰ������: 
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

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RUSSIANBRICK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RUSSIANBRICK));

	// ����Ϣѭ��: 
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
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //���ڱ���Ϊ��ɫ
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	char lpsz_0[] = "Operation Rules:";
	char lpsz_1[] = "press 'f' to start, press 'q'to quit, press 'p'or space to pause\0";
	char lpsz_2[] = "up--W down--S left--A right--D,or use the direction key,counterclockwise--Y,clockwise--U,  ��Ϸ�в����ִ�Сд\0";
	char lpsz_3[] = "rules:\0";
	char lpsz_4[] = "1.control the way of the brick falls down\0";
	char lpsz_5[] = "2.a line will disappear when it has been filled and the score will increase\0";
	char lpsz_6[] = "3.as the score goes high, the brick will fall down faster\0";
	char lpsz_7[] = "4.enjoy yourself\0";
	char lpsz_8[] = "5.once the fallen brick is stucked as soon as it comes out, the game is over\0";

	int X = 0, Y = 0;

	HFONT hF_black;	//��������������
	TEXTMETRIC tm;			//����һ��TEXTMETRIC�ṹ�����Լ�¼������Ϣ
	
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
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
		// TODO:  �ڴ���������ͼ����...
		hF_black = CreateFont		//�����Զ�������
			(
			15,				//����ĸ߶�
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

		SetBkColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, hF_black);		//���Զ�������ѡ���豸����
		GetTextMetrics(hdc, &tm);		//��ȡ�������Ϣ����д��tm�ṹ��

		SetTextColor(hdc, RGB(0, 255, 0));	//�����ı���ɫΪ��ɫ
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
		TextOutA(hdc, X, Y, lpsz_0, strlen(lpsz_0));	//ʹ�õ�ǰ��������ı�

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_1, strlen(lpsz_1));

		Y = Y + tm.tmHeight + tm.tmExternalLeading;//���㻻��ʱ��һ���ı����������
		TextOutA(hdc, X, Y,lpsz_2, strlen(lpsz_2));//ʹ�õ�ǰ��������ı�

		//���м�������ı����������е���ʼY����λ��
		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_3, strlen(lpsz_3));

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_4, strlen(lpsz_4));//use different way to output , W/A/TEXT

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_5, strlen(lpsz_5)); 	//����ı�

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y,lpsz_6, strlen(lpsz_6));//�Ե�ǰ��������ı�

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_7, strlen(lpsz_7));//�Ե�ǰ��������ı�

		Y = Y + tm.tmHeight + tm.tmExternalLeading;
		TextOutA(hdc, X, Y, lpsz_8, strlen(lpsz_8));//�Ե�ǰ��������ı�
		DeleteObject(hF_black);		//ɾ���Զ���������
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
		break;//����ط������׺��ԣ�����һ��������Ϣ�����EXIT;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

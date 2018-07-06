#include <windows.h>
#include "Snake.h"
#include<cstdio>
//#include "Snake.rc"

#define ID_TIMER 1
#define ID_TIMEOVER 2

Snake *sSnk;
int f_start=0;
//int f_over=0;
int f_pause=0;
HWND hwnd ;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM); 	//���ں���˵��

void moveDirect(Snake &sSnk){
	HWND hwnd=GetForegroundWindow();
	KillTimer(hwnd,ID_TIMER);
	sSnk.MoveSnake();
	if (!sSnk.GameOver())
	{
		sSnk.EatFood();
		sSnk.UnDrawSnake();
		sSnk.DrawSnake();
		sSnk.DrawFood();
	}
	else{
		SetTimer(hwnd,ID_TIMEOVER,5,NULL);
		return ;
	}
	SetTimer(hwnd,ID_TIMER,sSnk.speed,NULL);
	UpdateWindow(hwnd);
}

//------------ ���³�ʼ��������----------------
int WINAPI WinMain(HINSTANCE	hInstance,HINSTANCE hPrevInst,LPSTR lpszCmdLine,int	nCmdShow)
{
	MSG Msg ;
	WNDCLASS wndclass ;
	char lpszClassName[] = "Snake";			//��������
	char lpszTitle[]= "Snake";			//���ڱ�����

	//������Ķ���
	wndclass.style = 0;						//��������Ϊȱʡ����
	wndclass.lpfnWndProc = WndProc ;		//���ڴ�����ΪWndProc
	wndclass.cbClsExtra	= 0 ;				//����������չ
	wndclass.cbWndExtra	= 0 ;				//����ʵ������չ
	wndclass.hInstance = hInstance ;		//��ǰʵ�����
	wndclass.hIcon = LoadIcon( NULL, "ICON_SNAKE") ;
											//���ڵ���С��ͼ��Ϊȱʡͼ��
	wndclass.hCursor = LoadCursor( NULL,IDC_ARROW); //���ڲ��ü�ͷ���
	wndclass.hbrBackground =(HBRUSH) GetStockObject(BLACK_BRUSH); //���ڱ���Ϊ��ɫ
	wndclass.lpszMenuName = NULL ;			//�������޲˵�
	wndclass.lpszClassName = lpszClassName; 			//��������Ϊ"����ʾ��"

//--------------- ���½��д������ע�� -----------------
	if( !RegisterClass( &wndclass))			//���ע��ʧ���򷢳���������
	{
		//MessageBeep(0) ;
		return FALSE ;
	}

	//��������
	hwnd=CreateWindow(
			lpszClassName,		//��������
			lpszTitle,				//����ʵ���ı�����
			WS_OVERLAPPEDWINDOW,	//���ڵķ��
			CW_USEDEFAULT,
			CW_USEDEFAULT,			//�������Ͻ�����Ϊȱʡֵ
			600,
			500,			//���ڵĸߺͿ�Ϊȱʡֵ
			NULL,					//�˴����޸�����
			NULL,					//�˴��������˵�
			hInstance,				//�����˴��ڵ�Ӧ�ó���ĵ�ǰ���
			NULL) ;					//��ʹ�ø�ֵ

	ShowWindow( hwnd, nCmdShow); 			//��ʾ����
	UpdateWindow(hwnd); 					//�����û���

	while( GetMessage(&Msg, NULL, 0, 0))	//��Ϣѭ��
	{
		TranslateMessage( &Msg) ;
		DispatchMessage( &Msg) ;
	}
	return Msg.wParam;						//��Ϣѭ��������������ֹʱ����Ϣ����ϵͳ
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HDC hdc;
	HFONT hF_black;	//��������������
	PAINTSTRUCT ps;
	TEXTMETRIC tm;			//����һ��TEXTMETRIC�ṹ�����Լ�¼������Ϣ
	char lpsz_0[]="��������:";
	char lpsz_1[]="����'f'��ʼ��Ϸ,����'q'ֹͣ��Ϸ,����'p'��ո����ͣ��Ϸ";
	char lpsz_2[]="��--W ��--S ��--A ��--D, ����ʹ�÷����, ��Ϸ�в����ִ�Сд";
	char lpsz_3[]="����:";
	char lpsz_4[]="1. �տ�ʼ�����˶�;";
	char lpsz_5[]="2. ÿ��ֻ������ת������ת;";
	char lpsz_6[]="3. ÿ�Ե�4����Ʒ�ͻ����һ��������";
	char lpsz_7[]="4. ÿ����5��������һ��, ÿ��������ֵ+1, ��ֵ�ӱ�;";
	char lpsz_8[]="5. �����߽�����������ཻ�����߲�������������������Ϸ����!";
	int X=0,Y=MOVESTARTY+MOVEHEITH;
	//	SIZE size;					//����һ��SIZE���͵Ľṹ
	//GetTextExtentPoint32(hdc,lpsz_2,strlen(lpsz_2),&size);	//��ȡ�ַ����Ŀ��
      switch (message) 
      { 
		case WM_PAINT:
			hdc=BeginPaint(hwnd,&ps);
			hF_black=CreateFont		//�����Զ�������
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
					DEFAULT_PITCH|FF_DONTCARE,//�ּ�������ϵ��ʹ��ȱʡֵ
					"������"				//��������
					);

			SetTextColor(hdc,RGB(0,255,0));	//�����ı���ɫΪ��ɫ
			SetBkColor(hdc,RGB(0,0,0));
			SelectObject(hdc,hF_black);		//���Զ�������ѡ���豸����
			GetTextMetrics(hdc,&tm);		//��ȡ�������Ϣ����д��tm�ṹ��

			TextOut(hdc,X,Y,lpsz_0,strlen(lpsz_0));	//ʹ�õ�ǰ��������ı�

			Y=Y+tm.tmHeight+tm.tmExternalLeading;
			TextOut(hdc,X,Y,lpsz_1,strlen(lpsz_1));	

			Y=Y+tm.tmHeight+tm.tmExternalLeading;//���㻻��ʱ��һ���ı����������
			TextOut(hdc,X,Y,lpsz_2,strlen(lpsz_2));//ʹ�õ�ǰ��������ı�

			//���м�������ı����������е���ʼY����λ��
			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_3,strlen(lpsz_3));

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_4,strlen(lpsz_4));

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_5,strlen(lpsz_5)); 	//����ı�
			//�ڸ��м�������ı�
			
			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_6,strlen(lpsz_6));//�Ե�ǰ��������ı�

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_7,strlen(lpsz_7));//�Ե�ǰ��������ı�

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_8,strlen(lpsz_8));//�Ե�ǰ��������ı�
			
			EndPaint(hwnd,&ps);
			DeleteObject(hF_black);		//ɾ���Զ���������
			break;
        case WM_TIMER: 
			if (wParam==ID_TIMER)
			{
				moveDirect(*sSnk);
			}
			else if(wParam==ID_TIMEOVER){
				f_start=0;
				KillTimer(hwnd,ID_TIMEOVER);
				delete sSnk;
			}
			break;
		case WM_CHAR:
			switch (wParam)
			{
			case 'f':
			case 'F':
				if(!f_start){
					f_start=1;
					sSnk=new Snake;
					SetTimer(hwnd,ID_TIMER,(*sSnk).speed,NULL);
				}
				break;
			case 'p':
			case 'P':
				if(!f_start)break;
				f_pause=f_pause==0?1:0;
				if(f_pause){
					KillTimer(hwnd,ID_TIMER);
				}
				else{
					SetTimer(hwnd,ID_TIMER,(*sSnk).speed,NULL);
				}
				break;
			case 'w':
			case 'W':
				if(!f_start||f_pause)break;
				sSnk->direct=0;
				moveDirect(*sSnk);
				break;
			case 'd':
			case 'D':
				if(!f_start||f_pause)break;
				sSnk->direct=1;
				moveDirect(*sSnk);
				break;
			case 's':
			case 'S':
				if(!f_start||f_pause)break;
				sSnk->direct=2;
				moveDirect(*sSnk);
				break;
			case 'a':
			case 'A':
				if(!f_start||f_pause)break;
				sSnk->direct=3;
				moveDirect(*sSnk);
				break;
			case 'q':
			case 'Q':
				PostQuitMessage(0);
				break;
			case ' ':
				if(!f_start)break;
				f_pause=f_pause==0?1:0;
				if(f_pause){
					KillTimer(hwnd,ID_TIMER);
				}
				else{
					SetTimer(hwnd,ID_TIMER,(*sSnk).speed,NULL);
				}
			break;
			default:
				break;
			}
        case WM_KEYDOWN: 
			switch (wParam)
			{
			case VK_UP:
				if(!f_start||f_pause)break;
				sSnk->direct=0;
				moveDirect(*sSnk);
				break;
			case VK_RIGHT:
				if(!f_start||f_pause)break;
				sSnk->direct=1;
				moveDirect(*sSnk);
				break;
			case VK_DOWN:
				if(!f_start||f_pause)break;
				sSnk->direct=2;
				moveDirect(*sSnk);
				break;
			case VK_LEFT:
				if(!f_start||f_pause)break;
				sSnk->direct=3;
				moveDirect(*sSnk);
				break;
			
			default:
				break;
			}
			break;//����ط������׺��ԣ�����һ��������Ϣ�����EXIT;
		case WM_DESTROY:
			PostQuitMessage(0);					//����PostQuitMessage����WM_QUIT��Ϣ
			break;
		default:								//ȱʡʱ����ϵͳ��Ϣȱʡ������
			return  DefWindowProc(hwnd,message,wParam,lParam);
		}
		return (0);
}




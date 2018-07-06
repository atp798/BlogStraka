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

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM); 	//窗口函数说明

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

//------------ 以下初始化窗口类----------------
int WINAPI WinMain(HINSTANCE	hInstance,HINSTANCE hPrevInst,LPSTR lpszCmdLine,int	nCmdShow)
{
	MSG Msg ;
	WNDCLASS wndclass ;
	char lpszClassName[] = "Snake";			//窗口类名
	char lpszTitle[]= "Snake";			//窗口标题名

	//窗口类的定义
	wndclass.style = 0;						//窗口类型为缺省类型
	wndclass.lpfnWndProc = WndProc ;		//窗口处理函数为WndProc
	wndclass.cbClsExtra	= 0 ;				//窗口类无扩展
	wndclass.cbWndExtra	= 0 ;				//窗口实例无扩展
	wndclass.hInstance = hInstance ;		//当前实例句柄
	wndclass.hIcon = LoadIcon( NULL, "ICON_SNAKE") ;
											//窗口的最小化图标为缺省图标
	wndclass.hCursor = LoadCursor( NULL,IDC_ARROW); //窗口采用箭头光标
	wndclass.hbrBackground =(HBRUSH) GetStockObject(BLACK_BRUSH); //窗口背景为白色
	wndclass.lpszMenuName = NULL ;			//窗口中无菜单
	wndclass.lpszClassName = lpszClassName; 			//窗口类名为"窗口示例"

//--------------- 以下进行窗口类的注册 -----------------
	if( !RegisterClass( &wndclass))			//如果注册失败则发出警告声音
	{
		//MessageBeep(0) ;
		return FALSE ;
	}

	//创建窗口
	hwnd=CreateWindow(
			lpszClassName,		//窗口类名
			lpszTitle,				//窗口实例的标题名
			WS_OVERLAPPEDWINDOW,	//窗口的风格
			CW_USEDEFAULT,
			CW_USEDEFAULT,			//窗口左上角坐标为缺省值
			600,
			500,			//窗口的高和宽为缺省值
			NULL,					//此窗口无父窗口
			NULL,					//此窗口无主菜单
			hInstance,				//创建此窗口的应用程序的当前句柄
			NULL) ;					//不使用该值

	ShowWindow( hwnd, nCmdShow); 			//显示窗口
	UpdateWindow(hwnd); 					//绘制用户区

	while( GetMessage(&Msg, NULL, 0, 0))	//消息循环
	{
		TranslateMessage( &Msg) ;
		DispatchMessage( &Msg) ;
	}
	return Msg.wParam;						//消息循环结束即程序终止时将信息返回系统
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HDC hdc;
	HFONT hF_black;	//定义两种字体句柄
	PAINTSTRUCT ps;
	TEXTMETRIC tm;			//定义一个TEXTMETRIC结构，用以记录字体信息
	char lpsz_0[]="操作规则:";
	char lpsz_1[]="输入'f'开始游戏,输入'q'停止游戏,输入'p'或空格键暂停游戏";
	char lpsz_2[]="上--W 下--S 左--A 右--D, 或者使用方向键, 游戏中不区分大小写";
	char lpsz_3[]="规则:";
	char lpsz_4[]="1. 刚开始向右运动;";
	char lpsz_5[]="2. 每次只能向左转或向右转;";
	char lpsz_6[]="3. 每吃到4个物品就会出现一个“红心";
	char lpsz_7[]="4. 每吃完5个进入下一关, 每个基础分值+1, 分值加倍;";
	char lpsz_8[]="5. 碰到边界或者与自身相交或者蛇布满整个整个画面则游戏结束!";
	int X=0,Y=MOVESTARTY+MOVEHEITH;
	//	SIZE size;					//定义一个SIZE类型的结构
	//GetTextExtentPoint32(hdc,lpsz_2,strlen(lpsz_2),&size);	//获取字符串的宽度
      switch (message) 
      { 
		case WM_PAINT:
			hdc=BeginPaint(hwnd,&ps);
			hF_black=CreateFont		//创建自定义字体
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
					DEFAULT_PITCH|FF_DONTCARE,//字间距和字体系列使用缺省值
					"粗体字"				//字体名称
					);

			SetTextColor(hdc,RGB(0,255,0));	//设置文本颜色为绿色
			SetBkColor(hdc,RGB(0,0,0));
			SelectObject(hdc,hF_black);		//将自定义字体选入设备环境
			GetTextMetrics(hdc,&tm);		//获取字体的信息，并写入tm结构中

			TextOut(hdc,X,Y,lpsz_0,strlen(lpsz_0));	//使用当前字体输出文本

			Y=Y+tm.tmHeight+tm.tmExternalLeading;
			TextOut(hdc,X,Y,lpsz_1,strlen(lpsz_1));	

			Y=Y+tm.tmHeight+tm.tmExternalLeading;//计算换行时下一行文本的输出坐标
			TextOut(hdc,X,Y,lpsz_2,strlen(lpsz_2));//使用当前字体输出文本

			//换行继续输出文本，计算新行的起始Y坐标位置
			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_3,strlen(lpsz_3));

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_4,strlen(lpsz_4));

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_5,strlen(lpsz_5)); 	//输出文本
			//在该行继续输出文本
			
			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_6,strlen(lpsz_6));//以当前字体输出文本

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_7,strlen(lpsz_7));//以当前字体输出文本

			Y=Y+tm.tmHeight+tm.tmExternalLeading;	
			TextOut(hdc,X,Y,lpsz_8,strlen(lpsz_8));//以当前字体输出文本
			
			EndPaint(hwnd,&ps);
			DeleteObject(hF_black);		//删除自定义字体句柄
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
			break;//这个地方很容易忽略，导致一有其他消息程序就EXIT;
		case WM_DESTROY:
			PostQuitMessage(0);					//调用PostQuitMessage发出WM_QUIT消息
			break;
		default:								//缺省时采用系统消息缺省处理函数
			return  DefWindowProc(hwnd,message,wParam,lParam);
		}
		return (0);
}




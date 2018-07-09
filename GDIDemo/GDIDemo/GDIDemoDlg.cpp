
// GDIDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GDIDemo.h"
#include "GDIDemoDlg.h"
#include "DrawMethod.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGDIDemoDlg �Ի���


CGDIDemoDlg::CGDIDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGDIDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGDIDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGDIDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CGDIDemoDlg ��Ϣ�������

BOOL CGDIDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CPaintDC dc(this);
	dc.SetBkColor(RGB(0, 255, 0));
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGDIDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		RECT rec();
		//dc.Rectangle();
		CDrawMethod drawMethod(m_hWnd,&dc);
		drawMethod.BeginDraw();
		CRect rect(0,0,800,500);
		CRect rect1(10,10,50,50);
		CRect rect2(50,80,150,180);
		int offset = 10;
		int i;
		//situation 1\2
		/*
		for(i=0;i<1000;i++){
			drawMethod.FillRect(rect,RGB(250,250,250),255);
			drawMethod.DrawOnScreen();
			Sleep(1); //change it to simulate the different frequences
		}
		*/
		//situation 3
		/*
		left = 0;
		for(i=0;i<1000;i++){
			offset += 1;
			offset %=7;
			rect1.left = offset+10;
			rect1.right = offset+40;
			rect1.top = offset+10;
			rect1.bottom = offset + 40;


			drawMethod.FillRect(rect,RGB(0,0,0),255);
			drawMethod.FillRect(rect1,RGB(250,250,250),255);
			drawMethod.DrawOnScreen();
			Sleep(1);
		}
		*/
		//situation 4:
		/*
		for(i=0;i<1000;i++){
			drawMethod.FillRect(rect,RGB(0,0,0),255);
			drawMethod.FillRect(rect1,RGB(250,250,250),255);
			drawMethod.DrawOnScreen();
			Sleep(1);
			drawMethod.FillRect(rect,RGB(0,0,0),255);
			drawMethod.FillRect(rect1,RGB(170,0,0),255);
			drawMethod.DrawOnScreen();
			Sleep(50);//change this to simulate different proportions
		}
		*/

		//situation 5:
		/*
		for(i=0;i<1000;i++){
			offset += 1;
			offset %= 500;
			rect1.left = offset;
			rect1.right = offset+40;

			drawMethod.FillRect(rect,RGB(0,0,0),255);
			drawMethod.FillRect(rect1,RGB(250,250,250),255);
			drawMethod.DrawOnScreen();
			Sleep(10);//change this to control the moving speed
		}
		*/
		drawMethod.DrawRectFrame(rect1,1,RGB(0,255,0),150);
		drawMethod.DrawRectShadow(rect1,RGB(200,255,255),200,5);
		drawMethod.FillRect(rect1,RGB(255,0,0),150);

		drawMethod.DrawEllipseFrame(rect2,1,RGB(100,20,155),200);
		drawMethod.DrawEllipseShadow(rect2,RGB(0,255,255),200,5,0.7);
		drawMethod.FillEllipse(rect2,RGB(255,255,0),190);

		Gdiplus::Point pts[8];
		pts[0].X = 170; pts[0].Y = 250;
		pts[1].X = 280; pts[1].Y = pts[0].Y;
		pts[2].X = 400; pts[2].Y = 100;
		pts[3].X = 330; pts[3].Y = pts[2].Y;
		pts[4].X = pts[0].X; pts[4].Y = 210;
		pts[5].X = pts[1].X; pts[5].Y = pts[4].Y;
		pts[6].X = pts[2].X; pts[6].Y = 75;
		pts[7].X = pts[3].X; pts[7].Y = pts[6].Y;
		drawMethod.FillCubicSurface(pts,8,RGB(0,200,50),200,RGB(0,255,0));

		drawMethod.DrawOnScreen();
		drawMethod.EndDraw();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGDIDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CGDIDemoDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return true;
	return CDialogEx::OnEraseBkgnd(pDC);
}


void CGDIDemoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Invalidate(FALSE);
	UpdateWindow();
	CDialogEx::OnMouseMove(nFlags, point);
}


// USBCameraDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "USBCameraDemo.h"
#include "USBCameraDemoDlg.h"
#include "afxdialogex.h"
#include "videoInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned char *Scrbuffer;
int V_W = 640;
int V_H = 480;
bool m_bExit;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUSBCameraDemoDlg 对话框

CUSBCameraDemoDlg::CUSBCameraDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUSBCameraDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSBCameraDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUSBCameraDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUSBCameraDemoDlg 消息处理程序

BOOL CUSBCameraDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_bExit=false;
	InitializeCriticalSection(&m_csBuffer);
	m_hThread = CreateThread(NULL, 0, ThreadFun, this, 0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUSBCameraDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUSBCameraDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CDC m_dcMem;
		CBitmap m_bmpMem;
		CBitmap * m_pOldBmp;
		CRect m_rtClient;

		GetClientRect(&m_rtClient);
		m_dcMem.CreateCompatibleDC(&dc);
		Gdiplus::Graphics m_pGraph(m_dcMem.m_hDC);
		m_bmpMem.CreateCompatibleBitmap(&dc, m_rtClient.Width(), m_rtClient.Height());
		m_pOldBmp = m_dcMem.SelectObject(&m_bmpMem);
		m_pGraph.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//抗锯齿   

		EnterCriticalSection(&m_csBuffer);
		DrawPixels(m_dcMem,Scrbuffer, V_W, V_H);
		LeaveCriticalSection(&m_csBuffer);
		
		dc.BitBlt(0, 0, m_rtClient.Width(), m_rtClient.Height(), &m_dcMem, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台
		m_pGraph.ReleaseHDC(dc.GetSafeHdc());
		m_dcMem.SelectObject(m_pOldBmp);
		m_bmpMem.DeleteObject();
		m_dcMem.DeleteDC();
	}
}

int CUSBCameraDemoDlg::DrawPixels(CDC& m_dcMem,unsigned char* pPixels,int nWidth,int nHeight, bool bMirror){
	CDC memDC;
	memDC.CreateCompatibleDC(&m_dcMem);
	CRect m_rtClient;
	GetClientRect(&m_rtClient);
	
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(&m_dcMem, nWidth,nHeight);// , rect.Width(), rect.Height());
	//bmpMem.CreateBitmap(nWidth, nHeight, 1, 24, NULL);
	
	BITMAP bmp;
	bmpMem.GetBitmap(&bmp);
	int pixelBits = bmp.bmBitsPixel;
	int pixelBytes = pixelBits / 8;
	DWORD dwSize = bmp.bmHeight * bmp.bmWidthBytes;
	unsigned char* pBits = new unsigned char[dwSize];
	int i, j;
	for (i = 0; i < bmp.bmHeight; i++){
		for (j = 0; j < bmp.bmWidth; j++){
			//24bits format: BGR
			//32bits format: BGRA
			unsigned char * pPixelStart = pBits + i*bmp.bmWidthBytes + j* pixelBytes;
			unsigned char * pSrcStart = pPixels + i* bmp.bmWidth*3 + (bMirror?(bmp.bmWidth - j):j)*3;
			pPixelStart[0] = pSrcStart[0];
			pPixelStart[1] = pSrcStart[1];
			pPixelStart[2] = pSrcStart[2];
			pPixelStart[3] = 0;
		}
	}
	//bmpMem.SetBitmapBits(nWidth*nHeight*3, pPixels);
	bmpMem.SetBitmapBits(dwSize,pBits);
	CBitmap* pOldBmp = memDC.SelectObject(&bmpMem);
	
	m_dcMem.StretchBlt(0, 0, m_rtClient.Width(), m_rtClient.Height(), &memDC,0,0,nWidth ,nHeight,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	delete[] pBits;
	bmpMem.DeleteObject();
	memDC.DeleteDC();
	//DrawImage("Capture1.jpg", m_rtClient);
	//m_dcMem.SetBkColor(RGB(0, 255, 255));
	return 0;
}

void CUSBCameraDemoDlg::VdoFrameData(double dblSampleTime, BYTE * pBuffer, long lBufferSize, void *Temp){
	Invalidate(FALSE);
	UpdateWindow();
}

DWORD WINAPI CUSBCameraDemoDlg::ThreadFun(LPVOID param)
{
	CUSBCameraDemoDlg *pDlg = (CUSBCameraDemoDlg *)param;
	HWND hWnd = pDlg->m_hWnd;

	unsigned char *Buffer = new unsigned char[V_W*V_H * 3];
	Scrbuffer = Buffer;
	videoInput video;
	int deviceID = 0;//the index of camera, if there is only one camera, deviceID is 0

	if (!video.setupDevice(deviceID, V_W, V_H))
	{
		CString strMsg("无法打开摄像头！");
		AfxMessageBox(strMsg);
	}

	while (true)
	{
		if (video.isFrameNew(deviceID))
		{
			EnterCriticalSection(&(pDlg->m_csBuffer));
			if(m_bExit){ 
				LeaveCriticalSection(&(pDlg->m_csBuffer));
				break;
			}
			video.getPixels(deviceID, Buffer, false, true);//get pixels of a frame
			LeaveCriticalSection(&(pDlg->m_csBuffer));
			pDlg->VdoFrameData(0, Buffer, V_W*V_H * 3, param);
		}
	}
	//you must stop device here before thread exit, otherwise it will cause some system problems
	//and the thread will not release the camera resources 
	//which makes any other thread can't open the camera even untill the system restarts
	video.stopDevice(deviceID);
	delete[]Buffer;
	return 1;
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUSBCameraDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CUSBCameraDemoDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CUSBCameraDemoDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	DeleteCriticalSection(&m_csBuffer);
	return CDialogEx::DestroyWindow();
}


void CUSBCameraDemoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	EnterCriticalSection(&m_csBuffer);
	m_bExit=true;
	LeaveCriticalSection(&m_csBuffer);
	if(WaitForSingleObject(m_hThread,2000)==WAIT_TIMEOUT){ //timewait bigger enough to make sure the thread exit will camera stopped
		TerminateThread(m_hThread,0);
		CloseHandle(m_hThread);
	}
	CDialogEx::OnClose();
}

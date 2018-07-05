
// USBCameraDemoDlg.h : 头文件
//

#pragma once


// CUSBCameraDemoDlg 对话框
class CUSBCameraDemoDlg : public CDialogEx
{
private:
	HANDLE m_hThread;
	CRITICAL_SECTION m_csBuffer;
// 构造
public:
	CUSBCameraDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USBCAMERADEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	int CUSBCameraDemoDlg::DrawPixels(CDC& m_dcMem,unsigned char* pPixels,int nWidth,int nHeight, bool bMirror=false);
	static DWORD WINAPI ThreadFun(LPVOID param);
	void VdoFrameData(double dblSampleTime, BYTE * pBuffer, long lBufferSize, void *Temp);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL DestroyWindow();
	afx_msg void OnClose();
};

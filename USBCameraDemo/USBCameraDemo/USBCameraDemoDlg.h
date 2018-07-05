
// USBCameraDemoDlg.h : ͷ�ļ�
//

#pragma once


// CUSBCameraDemoDlg �Ի���
class CUSBCameraDemoDlg : public CDialogEx
{
private:
	HANDLE m_hThread;
	CRITICAL_SECTION m_csBuffer;
// ����
public:
	CUSBCameraDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_USBCAMERADEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	
	int CUSBCameraDemoDlg::DrawPixels(CDC& m_dcMem,unsigned char* pPixels,int nWidth,int nHeight, bool bMirror=false);
	static DWORD WINAPI ThreadFun(LPVOID param);
	void VdoFrameData(double dblSampleTime, BYTE * pBuffer, long lBufferSize, void *Temp);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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


// USBCameraDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUSBCameraDemoApp:
// �йش����ʵ�֣������ USBCameraDemo.cpp
//

class CUSBCameraDemoApp : public CWinApp
{
public:
	CUSBCameraDemoApp();
	ULONG_PTR m_gdiplusToken;

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CUSBCameraDemoApp theApp;
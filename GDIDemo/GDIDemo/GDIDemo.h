
// GDIDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGDIDemoApp:
// �йش����ʵ�֣������ GDIDemo.cpp
//

class CGDIDemoApp : public CWinApp
{
public:
	CGDIDemoApp();

// ��д
public:
	virtual BOOL InitInstance();
	ULONG_PTR m_gdiplusToken;

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CGDIDemoApp theApp;
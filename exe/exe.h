// exe.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������
#include "fltrule.h"
#include "apprule.h"

// CPfwApp:
// �йش����ʵ�֣������ exe.cpp
//

class CPfwApp : public CWinApp
{
public:
	CPfwApp();

public:
	CFilterRules	m_FilterRules;
	CAppRules		m_AppRules;

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��
	void GetAppPath( LPTSTR lpszPath, DWORD nSize );

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CPfwApp theApp;

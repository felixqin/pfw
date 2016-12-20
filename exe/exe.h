// exe.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#include "fltrule.h"
#include "apprule.h"

// CPfwApp:
// 有关此类的实现，请参阅 exe.cpp
//

class CPfwApp : public CWinApp
{
public:
	CPfwApp();

public:
	CFilterRules	m_FilterRules;
	CAppRules		m_AppRules;

// 重写
	public:
	virtual BOOL InitInstance();

// 实现
	void GetAppPath( LPTSTR lpszPath, DWORD nSize );

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CPfwApp theApp;

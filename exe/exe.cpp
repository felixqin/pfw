// exe.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "exe.h"
#include "PfwDlg.h"
#include ".\exe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPfwApp

BEGIN_MESSAGE_MAP(CPfwApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPfwApp 构造

CPfwApp::CPfwApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPfwApp 对象

CPfwApp theApp;


// CPfwApp 初始化

BOOL CPfwApp::InitInstance()
{
	OleInitialize( NULL );

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	if ( !PfwInitialize() )
	{
		AfxMessageBox( IDS_MB_ERROR_INITPFW, MB_ICONERROR );
		return FALSE;
	}

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Flysoft"));

	CPfwDlg dlg;
	TCHAR szAppPath[MAX_PATH];
	CString szFNFilterRule;
	CString szFNAppRule;

	// 变量初始化
	BOOL succ = TRUE;

	GetAppPath( szAppPath, MAX_PATH );

	szFNFilterRule.LoadString( IDS_FN_FILTERRULE );
	szFNFilterRule = szAppPath + szFNFilterRule;
	if ( m_FilterRules.LoadFromFile( szFNFilterRule ) < 0 )
	{
		succ = FALSE;
		goto done;
	}

	szFNAppRule.LoadString( IDS_FN_APPRULE );
	szFNAppRule = szAppPath + szFNAppRule;
	if ( m_AppRules.LoadFromFile(szFNAppRule) < 0 )
	{
		succ = FALSE;
		goto done;
	}

	//运行主窗口
	m_pMainWnd = &dlg;
	dlg.DoModal();

done:	//退出处理
	if (succ)
	{
		m_FilterRules.SaveToFile(szFNFilterRule);
		m_AppRules.SaveToFile(szFNAppRule);
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}
void CPfwApp::GetAppPath( LPTSTR lpszPath, DWORD nSize )
{
	TCHAR szAppName[MAX_PATH];
	LPTSTR lpszNamePart;

	GetModuleFileName( NULL, szAppName, MAX_PATH );
	GetFullPathName( szAppName, nSize, lpszPath, &lpszNamePart );
	*lpszNamePart = _T('\0');
}
int CPfwApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	PfwUninitialize();
	OleUninitialize();

	return CWinApp::ExitInstance();
}

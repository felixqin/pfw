// exe.cpp : ����Ӧ�ó��������Ϊ��
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


// CPfwApp ����

CPfwApp::CPfwApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPfwApp ����

CPfwApp theApp;


// CPfwApp ��ʼ��

BOOL CPfwApp::InitInstance()
{
	OleInitialize( NULL );

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	if ( !PfwInitialize() )
	{
		AfxMessageBox( IDS_MB_ERROR_INITPFW, MB_ICONERROR );
		return FALSE;
	}

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Flysoft"));

	CPfwDlg dlg;
	TCHAR szAppPath[MAX_PATH];
	CString szFNFilterRule;
	CString szFNAppRule;

	// ������ʼ��
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

	//����������
	m_pMainWnd = &dlg;
	dlg.DoModal();

done:	//�˳�����
	if (succ)
	{
		m_FilterRules.SaveToFile(szFNFilterRule);
		m_AppRules.SaveToFile(szFNAppRule);
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
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
	// TODO: �ڴ����ר�ô����/����û���
	PfwUninitialize();
	OleUninitialize();

	return CWinApp::ExitInstance();
}

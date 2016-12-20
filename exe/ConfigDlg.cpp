// ConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <atlbase.h>
#include "exe.h"
#include "ConfigDlg.h"
#include ".\configdlg.h"


// CConfigDlg �Ի���

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)
CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
	, m_bAutoStart(FALSE)
{
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_AUTO_START, m_bAutoStart);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_CMD_APPLY_CONFIG, OnCmdApply)
	ON_BN_CLICKED(IDC_CMD_RESTORE_CONFIG, OnCmdRestore)
END_MESSAGE_MAP()


// CConfigDlg ��Ϣ�������

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CRegKey reg;
	if (  ERROR_SUCCESS == reg.Create( HKEY_CURRENT_USER,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run") ) )
	{
		TCHAR szValue[MAX_PATH];
		TCHAR szAppName[MAX_PATH];
		ULONG cc = MAX_PATH;
		CString szValueName;
		szValueName.LoadString( IDS_REGKEY_AUTORUN );
		if ( reg.QueryStringValue(szValueName,szValue,&cc) == ERROR_SUCCESS )
		{
			GetModuleFileName( NULL, szAppName, MAX_PATH );
			m_bAutoStart = ( _tcsicmp( szAppName, szValue ) == 0 );
		}
		reg.Close();
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CConfigDlg::OnCmdApply()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	CRegKey reg;
	if (  ERROR_SUCCESS == reg.Create( HKEY_CURRENT_USER,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run") ) )
	{
		CString szValueName;
		szValueName.LoadString( IDS_REGKEY_AUTORUN );
		if ( m_bAutoStart )
		{
			TCHAR szAppName[MAX_PATH];
			GetModuleFileName( NULL, szAppName, MAX_PATH );
			reg.SetValue( szAppName, szValueName );
		}
		else
		{
			reg.DeleteValue( szValueName );
		}
		reg.Close();
	}
}

void CConfigDlg::OnCmdRestore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(FALSE);
}
/*****************************************************************
���ضԻ������ OnOK() �� OnCancel() ��ֹ�û��رմ˶Ի���
*****************************************************************/
void CConfigDlg::OnOK()
{}
void CConfigDlg::OnCancel()
{}

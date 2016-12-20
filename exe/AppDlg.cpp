// AppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "exe.h"
#include "pfw_ui.h"
#include "AppDlg.h"
#include "ARuleInfoDlg.h"
#include ".\appdlg.h"



// CAppDlg �Ի���

IMPLEMENT_DYNAMIC(CAppDlg, CDialog)
CAppDlg::CAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppDlg::IDD, pParent),
	m_pAppRules( &theApp.m_AppRules )	//ָ�������ʹ���
{
}

CAppDlg::~CAppDlg()
{
}

void CAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APP_LIST, m_AppList);
}


BEGIN_MESSAGE_MAP(CAppDlg, CDialog)
	ON_BN_CLICKED(IDC_CMD_MODIFY_APP, OnCmdModify)
	ON_BN_CLICKED(IDC_CMD_DELETE_APP, OnCmdDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_APP_LIST, OnNMDblclkAppList)
END_MESSAGE_MAP()


// CAppDlg ��Ϣ�������

BOOL CAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	InitAppList();

	const APP_RULE *rule = m_pAppRules->First();
	while ( rule )
	{
		InsertListItem( rule->app_name );
		rule = m_pAppRules->Next();
	}
	////test
	//OnAppRuleInsert( CARule(_T("G:\\MyPrjs\\Net\\sr\\bin\\Client.exe")) );
	//OnAppRuleInsert( CARule(_T("D:\\GreenSoft\\Debug View\\Dbgview.exe")) );

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CAppDlg::InitAppList()
{
	m_AppList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	//�����б�ؼ��� ImageList
	SHFILEINFO sfi;
	HIMAGELIST hSystemSmallImageList 
		= (HIMAGELIST)SHGetFileInfo( (LPCTSTR)_T("C:\\"), 0, &sfi,
				sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	m_AppImage.Attach( hSystemSmallImageList );
	m_AppList.SetImageList( &m_AppImage, LVSIL_SMALL );

	//�����б�ؼ��ı�ͷ
	InsertListColumn( m_AppList, 0, IDS_LH_NAME, 150 );
	InsertListColumn( m_AppList, 1, IDS_LH_PATH, 350 );
}
/*
���б�ؼ������Ӧ�ó�����Ϣ
*/
void CAppDlg::InsertListItem( LPCTSTR szFileName )
{
	//ȡ���ļ���ͼ������
	SHFILEINFO    sfi;
	SHGetFileInfo( szFileName, 0, &sfi,	sizeof(SHFILEINFO), 
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	int nImage = sfi.iIcon;

	//ȡ���ļ�����·��
	TCHAR szPathName[MAX_PATH];
	LPTSTR szName;
	GetFullPathName( szFileName, MAX_PATH, szPathName, &szName );

	//���б�������ļ���Ϣ
	m_AppList.InsertItem( 0, szName, nImage );
	m_AppList.SetItemText( 0, 1, szPathName );
}
//��������¼�, ��������Ի������
void CAppDlg::OnAppRuleInsert(const CARule &rule)
{
	//������򵽳�����ʹ������
	if (m_pAppRules->Insert(rule))
	{
		InsertListItem( rule.GetRule()->app_name );
	}
}
//�����޸��¼�
void CAppDlg::OnCmdModify()
{
	POSITION pos = m_AppList.GetFirstSelectedItemPosition();
	int nItem = m_AppList.GetNextSelectedItem( pos );
	if ( nItem < 0 ) return;

	CString szFileName = m_AppList.GetItemText( nItem, 1 );
	const APP_RULE* pRule = m_pAppRules->GetRule( szFileName );
	if ( pRule )
	{
		CARuleInfoDlg dlgInfo;
		dlgInfo.m_Rule = *pRule;
		if ( dlgInfo.DoModal() == IDOK
			&& !(dlgInfo.m_Rule == *pRule) )
		{
			m_pAppRules->Modify( dlgInfo.m_Rule );
		}
	}
}
void CAppDlg::OnNMDblclkAppList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCmdModify();
	*pResult = 0;
}
//����ɾ���¼�
void CAppDlg::OnCmdDelete()
{
	POSITION pos = m_AppList.GetFirstSelectedItemPosition();
	int nItem = m_AppList.GetNextSelectedItem( pos );
	if ( nItem < 0 ) return;

	CString szFileName = m_AppList.GetItemText( nItem, 1 );
	if ( AfxMessageBox(IDS_MB_SURE_DELETE,MB_YESNO|MB_ICONQUESTION) == IDYES )
	{
		if (m_pAppRules->Delete( szFileName ) )
		{
			m_AppList.DeleteItem( nItem );
		}
		else
		{
			AfxMessageBox( IDS_MB_DELETE_FAILURE, MB_ICONERROR );
		}
	}
}

/*****************************************************************
���ضԻ������ OnOK() �� OnCancel() ��ֹ�û��رմ˶Ի���
*****************************************************************/
void CAppDlg::OnOK()
{
}
void CAppDlg::OnCancel()
{
}


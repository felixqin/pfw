// AppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "exe.h"
#include "pfw_ui.h"
#include "AppDlg.h"
#include "ARuleInfoDlg.h"
#include ".\appdlg.h"



// CAppDlg 对话框

IMPLEMENT_DYNAMIC(CAppDlg, CDialog)
CAppDlg::CAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppDlg::IDD, pParent),
	m_pAppRules( &theApp.m_AppRules )	//指向程序访问规则
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


// CAppDlg 消息处理程序

BOOL CAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

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
	// 异常: OCX 属性页应返回 FALSE
}
void CAppDlg::InitAppList()
{
	m_AppList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	//设置列表控件的 ImageList
	SHFILEINFO sfi;
	HIMAGELIST hSystemSmallImageList 
		= (HIMAGELIST)SHGetFileInfo( (LPCTSTR)_T("C:\\"), 0, &sfi,
				sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	m_AppImage.Attach( hSystemSmallImageList );
	m_AppList.SetImageList( &m_AppImage, LVSIL_SMALL );

	//设置列表控件的表头
	InsertListColumn( m_AppList, 0, IDS_LH_NAME, 150 );
	InsertListColumn( m_AppList, 1, IDS_LH_PATH, 350 );
}
/*
向列表控件中添加应用程序信息
*/
void CAppDlg::InsertListItem( LPCTSTR szFileName )
{
	//取得文件的图标索引
	SHFILEINFO    sfi;
	SHGetFileInfo( szFileName, 0, &sfi,	sizeof(SHFILEINFO), 
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	int nImage = sfi.iIcon;

	//取得文件名和路径
	TCHAR szPathName[MAX_PATH];
	LPTSTR szName;
	GetFullPathName( szFileName, MAX_PATH, szPathName, &szName );

	//向列表中添加文件信息
	m_AppList.InsertItem( 0, szName, nImage );
	m_AppList.SetItemText( 0, 1, szPathName );
}
//处理添加事件, 由主界面对话框调用
void CAppDlg::OnAppRuleInsert(const CARule &rule)
{
	//插入规则到程序访问规则表中
	if (m_pAppRules->Insert(rule))
	{
		InsertListItem( rule.GetRule()->app_name );
	}
}
//处理修改事件
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
	// TODO: 在此添加控件通知处理程序代码
	OnCmdModify();
	*pResult = 0;
}
//处理删除事件
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
重载对话框类的 OnOK() 和 OnCancel() 防止用户关闭此对话框
*****************************************************************/
void CAppDlg::OnOK()
{
}
void CAppDlg::OnCancel()
{
}


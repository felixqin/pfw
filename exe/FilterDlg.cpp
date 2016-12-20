// FilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "exe.h"
#include "pfw_ui.h"
#include "FRuleInfoDlg.h"
#include "FilterDlg.h"
#include ".\filterdlg.h"


// CFilterDlg 对话框

IMPLEMENT_DYNAMIC(CFilterDlg, CDialog)
CFilterDlg::CFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent)
	, m_pFilterRules( &theApp.m_FilterRules )
{
}

CFilterDlg::~CFilterDlg()
{
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILTER_LIST, m_FilterList);
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	ON_BN_CLICKED(IDC_CMD_FILTER_INSERT, OnCmdInsert)
	ON_BN_CLICKED(IDC_CMD_FILTER_DELETE, OnCmdDelete)
	ON_BN_CLICKED(IDC_CMD_FILTER_MODIFY, OnCmdModify)
	ON_BN_CLICKED(IDC_CMD_FILTER_MOVEUP, OnCmdMoveUp)
	ON_BN_CLICKED(IDC_CMD_FILTER_MOVE_DOWN, OnCmdMoveDown)
	ON_NOTIFY(NM_DBLCLK, IDC_FILTER_LIST, OnNMDblclkFilterList)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序

BOOL CFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitFilterList();

	int nItem = 0;
	const FILTER_RULE *rule = m_pFilterRules->First();
	while ( rule )
	{
		InsertListItem(nItem++, rule->action, rule->name, rule->description);
		rule = m_pFilterRules->Next();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CFilterDlg::InitFilterList()
{
	m_FilterList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ImageList.Create( 16, 16, ILC_COLOR4, 0, 1 );
	m_ImageList.SetBkColor( m_FilterList.GetBkColor() );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_ACTION_PASS) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_ACTION_DENY) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_ACTION_CONTINUE) );
	m_FilterList.SetImageList( &m_ImageList, LVSIL_SMALL );

	m_FilterList.InsertColumn( 0, NULL, LVCFMT_LEFT, 20 );
	InsertListColumn( m_FilterList, 1, IDS_LH_FILTER_ACTION, 40 );
	InsertListColumn( m_FilterList, 2, IDS_LH_FILTER_NAME, 150 );
	InsertListColumn( m_FilterList, 3, IDS_LH_FILTER_DESCRIPTION, 300 );
}
void CFilterDlg::InsertListItem(int nItem,USHORT action,LPCTSTR name,LPCTSTR desc)
{
	int iImage;	UINT uID;
	if ( IS_ACTION_CONTINUE(action) )
	{
		iImage = 2;
		uID = IDS_ACTION_CONTINUE;
	}
	else if ( IS_ACTION_PASS(action) )
	{
		iImage = 0;
		uID = IDS_ACTION_PASS;
	}
	else
	{
		iImage = 1;
		uID = IDS_ACTION_DENY;
	}
	CString txt; txt.LoadString( uID );

	//图标//动作//名称和说明
	m_FilterList.InsertItem( nItem, NULL, iImage );
	m_FilterList.SetItemText( nItem, 1, txt );
	m_FilterList.SetItemText( nItem, 2, name );
	m_FilterList.SetItemText( nItem, 3, desc );
}
void CFilterDlg::UpdateListItem(int nItem,USHORT action,LPCTSTR name,LPCTSTR desc)
{
	int iImage;	UINT uID;
	if ( IS_ACTION_CONTINUE(action) )
	{
		iImage = 2;
		uID = IDS_ACTION_CONTINUE;
	}
	else if ( IS_ACTION_PASS(action) )
	{
		iImage = 0;
		uID = IDS_ACTION_PASS;
	}
	else
	{
		iImage = 1;
		uID = IDS_ACTION_DENY;
	}
	CString txt; txt.LoadString( uID );

	//图标//动作//名称和说明
	m_FilterList.SetItem( nItem, 0, LVIF_IMAGE, NULL, iImage, 0, 0, 0 );
	m_FilterList.SetItemText( nItem, 1, txt );
	m_FilterList.SetItemText( nItem, 2, name );
	m_FilterList.SetItemText( nItem, 3, desc );
}
/////////////////////////////////////////////////////////////////////////////////
// 命令消息处理

void CFilterDlg::OnCmdInsert()
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	int nItem = m_FilterList.GetNextSelectedItem( pos );
	if ( nItem < 0 ) nItem = m_FilterList.GetItemCount();

	CFRuleInfoDlg dlgInfo;
	if ( dlgInfo.DoModal() == IDOK )
	{
		CFRule &rule = dlgInfo.m_Rule;
		if ( m_pFilterRules->InsertAfter( rule, nItem-1 ) )
		{
			InsertListItem( nItem, rule.GetRule()->action, 
					rule.GetRule()->name, rule.GetRule()->description );
			m_FilterList.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
			m_FilterList.EnsureVisible( nItem, TRUE );
		}
	}
}

void CFilterDlg::OnCmdDelete()
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	int nItem = m_FilterList.GetNextSelectedItem( pos );
	if ( nItem < 0 ) return;

	if (AfxMessageBox(IDS_MB_SURE_DELETE, MB_YESNO|MB_ICONQUESTION)==IDYES)
	{
		if ( m_pFilterRules->Delete(nItem) )
		{
			m_FilterList.DeleteItem( nItem );
		}
	}
}

void CFilterDlg::OnCmdModify()
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	int nItem = m_FilterList.GetNextSelectedItem( pos );
	if ( nItem < 0 ) return;

	const FILTER_RULE *pRule = m_pFilterRules->GetRule( nItem );
	if ( NULL == pRule ) return;

	CFRuleInfoDlg dlgInfo;
	dlgInfo.m_Rule = *pRule;
	if ( dlgInfo.DoModal() == IDOK )
	{
		CFRule &rule = dlgInfo.m_Rule;
		if ( !( rule == *pRule )
			&& m_pFilterRules->Modify( rule, nItem ) )
		{
			UpdateListItem( nItem, rule.GetRule()->action, 
					rule.GetRule()->name, rule.GetRule()->description );
		}
		////TEST
		//UpdateListItem( nItem, ACTION_DENY, _T("AA"), _T("BBB") );
	}
}

void CFilterDlg::OnCmdMoveUp()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	int nItem = m_FilterList.GetNextSelectedItem( pos );
	if ( nItem <= 0 ) return;

	if ( m_pFilterRules->MoveUp( nItem ) )
	{
		int p1 = nItem-1, p2 = nItem;
		const FILTER_RULE *r1 = m_pFilterRules->GetRule( p1 );
		const FILTER_RULE *r2 = m_pFilterRules->GetRule( p2 );
		
		UpdateListItem( p1, r1->action, r1->name, r1->description );
		UpdateListItem( p2, r2->action, r2->name, r2->description );

		m_FilterList.SetItemState( p1, LVIS_SELECTED, LVIS_SELECTED );
		m_FilterList.EnsureVisible( p1, TRUE );
	}
}

void CFilterDlg::OnCmdMoveDown()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	int nItem = m_FilterList.GetNextSelectedItem( pos );
	if ( nItem < 0 || nItem == m_FilterList.GetItemCount()-1 )
		return;

	if ( m_pFilterRules->MoveDown( nItem ) )
	{
		int p1 = nItem+1, p2 = nItem;
		const FILTER_RULE *r1 = m_pFilterRules->GetRule( p1 );
		const FILTER_RULE *r2 = m_pFilterRules->GetRule( p2 );
		
		UpdateListItem( p1, r1->action, r1->name, r1->description );
		UpdateListItem( p2, r2->action, r2->name, r2->description );

		m_FilterList.SetItemState( p1, LVIS_SELECTED, LVIS_SELECTED );
		m_FilterList.EnsureVisible( p1, TRUE );
	}
}

void CFilterDlg::OnNMDblclkFilterList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	OnCmdModify();
	*pResult = 0;
}

/*****************************************************************
重载对话框类的 OnOK() 和 OnCancel() 防止用户关闭此对话框
*****************************************************************/
void CFilterDlg::OnOK()
{
}
void CFilterDlg::OnCancel()
{
}

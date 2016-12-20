// LogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "exe.h"
#include "pfw_ui.h"
#include "LogDlg.h"
#include ".\logdlg.h"
#include <fstream>

using namespace std;

// CLogDlg 对话框

IMPLEMENT_DYNAMIC(CLogDlg, CDialog)
CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDlg::IDD, pParent)
	, m_bScroll(TRUE)
	, m_bLogEnable(TRUE)
{
}

CLogDlg::~CLogDlg()
{
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_LIST, m_LogList);
}


BEGIN_MESSAGE_MAP(CLogDlg, CDialog)
//	ON_WM_DESTROY()
ON_BN_CLICKED(IDC_CMD_MONITOR_LOG, OnCmdLogEnable)
ON_BN_CLICKED(IDC_CMD_SCROLL_LOG, OnCmdScrollLog)
ON_BN_CLICKED(IDC_CMD_CLEAR_LOG, OnCmdClearLog)
ON_BN_CLICKED(IDC_CMD_SAVE_LOG, OnCmdSaveLog)
END_MESSAGE_MAP()


// CLogDlg 消息处理程序

BOOL CLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitLogList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CLogDlg::InitLogList()
{
	m_LogList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ImageList.Create( 16, 16, ILC_COLOR4, 0, 1 );
	m_ImageList.SetBkColor( m_LogList.GetBkColor() );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_ACTION_PASS) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_ACTION_DENY) );
	m_LogList.SetImageList( &m_ImageList, LVSIL_SMALL );

	m_LogList.InsertColumn( 0, NULL, LVCFMT_LEFT, 20 );
	InsertListColumn( m_LogList, 1, IDS_LH_LOG_ACTION, 40 );
	InsertListColumn( m_LogList, 2, IDS_LH_LOG_TIME, 70 );
	InsertListColumn( m_LogList, 3, IDS_LH_LOG_NAME, 80 );
	InsertListColumn( m_LogList, 4, IDS_LH_LOG_PROTO, 40 );
	InsertListColumn( m_LogList, 5, IDS_LH_LOG_LPORT, 60 );
	InsertListColumn( m_LogList, 6, IDS_LH_LOG_DIRECT, 36 );
	InsertListColumn( m_LogList, 7, IDS_LH_LOG_RIP, 110 );
	InsertListColumn( m_LogList, 8, IDS_LH_LOG_RPORT, 60 );
}
//处理插入日志
void CLogDlg::InsertLogToList( const LOG_INFO &log )
{
	CString txt;
	int nItem = m_LogList.GetItemCount();

	//图标
	int nImage = IS_ACTION_PASS(log.action) ? 0 : 1;
	m_LogList.InsertItem( nItem, NULL, nImage );

	//动作
	UINT uID = IS_ACTION_PASS(log.action) ?
				IDS_ACTION_PASS : IDS_ACTION_DENY;
	txt.LoadString( uID );
	m_LogList.SetItemText( nItem, 1, txt );

	//时间
	CTime t = CTime::GetCurrentTime();
	txt = t.Format( IDS_FORMAT_TIME );
	m_LogList.SetItemText( nItem, 2, txt );

	//程序
	TCHAR szFileName[MAX_PATH];
	GetProcessFileName( log.pid, szFileName, MAX_PATH );
	TCHAR szFullName[MAX_PATH];
	TCHAR *lpszName;
	GetFullPathName( szFileName, MAX_PATH, szFullName, &lpszName );
	m_LogList.SetItemText( nItem, 3, lpszName );

	//协议
	if ( log.type & RT_TCP )
	{
		txt = _T("TCP");
	}
	else if ( log.type & RT_UDP )
	{
		txt = _T("UDP");
	}
	else
	{
		txt = _T("IP");
	}
	m_LogList.SetItemText( nItem, 4, txt );

	//本地端口
	txt.Format( _T("%d"), log.lport );
	m_LogList.SetItemText( nItem, 5, txt );

	//方向
	uID = IS_DIRECTIN(log.type) ? IDS_DIRECT_IN : IDS_DIRECT_OUT;
	txt.LoadString( uID );
	m_LogList.SetItemText( nItem, 6, txt );

	//对方IP
	BYTE *ip = (BYTE*)(&log.ip);
	txt.Format( _T("%d.%d.%d.%d"), ip[3], ip[2], ip[1], ip[0] );
	m_LogList.SetItemText( nItem, 7, txt );

	//对方端口
	txt.Format( _T("%d"), log.rport );
	m_LogList.SetItemText( nItem, 8, txt );
}
int CLogDlg::SaveLogList( LPCTSTR lpszFileName )
{
	ofstream out;

	USES_CONVERSION;
	out.open( T2A(lpszFileName) );

	int cc = m_LogList.GetItemCount();
	for ( int i = 0; i<cc; i++ )
	{
		for ( int j = 1; j<=8; j++ )
		{
			CString txt = m_LogList.GetItemText( i, j );
			out<<T2A(txt)<<'\t';
		}
		out<<endl;
	}

	out.close();
	return cc;
}
void CLogDlg::OnInsertLog( const LOG_INFO &log )
{
	if ( m_bLogEnable )
	{
		InsertLogToList( log );
		if ( m_bScroll )
		{
			m_LogList.EnsureVisible( m_LogList.GetItemCount()-1, FALSE );
		}
	}
}
void CLogDlg::OnCmdLogEnable()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bLogEnable = !m_bLogEnable;
	UINT uID = m_bLogEnable ? IDS_BTNTXT_LOGDISABLE : IDS_BTNTXT_LOGENABLE;
	CString szCaption;
	szCaption.LoadString( uID );
	SetDlgItemText( IDC_CMD_MONITOR_LOG, szCaption );
}

void CLogDlg::OnCmdScrollLog()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bScroll = !m_bScroll;
	UINT uID = m_bScroll ? IDS_BTNTXT_SCROLLDISABLE : IDS_BTNTXT_SCROLLENABLE;
	CString szCaption;
	szCaption.LoadString( uID );
	SetDlgItemText( IDC_CMD_SCROLL_LOG, szCaption );
}

void CLogDlg::OnCmdClearLog()
{
	// TODO: 在此添加控件通知处理程序代码
	m_LogList.DeleteAllItems();
}

void CLogDlg::OnCmdSaveLog()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szFilters;
	szFilters.LoadString( IDS_OFN_FILTER_LOG );
	CFileDialog dlgFile( FALSE , _T("LOG"), _T("*.LOG"),
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if ( dlgFile.DoModal() == IDOK )
	{
		SaveLogList( dlgFile.GetPathName() );
	}
}

/*****************************************************************
重载对话框类的 OnOK() 和 OnCancel() 防止用户关闭此对话框
*****************************************************************/
void CLogDlg::OnOK()
{
}
void CLogDlg::OnCancel()
{
}

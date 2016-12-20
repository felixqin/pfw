// PfwDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <mmsystem.h>
#include "exe.h"
#include "PfwDlg.h"
#include ".\pfwdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPfwDlg �Ի���



CPfwDlg::CPfwDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPfwDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_idNotifyIcon = ID_NOTIFYICON;
}

void CPfwDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLG_CTRL, m_DlgCtrl);
	DDX_Control(pDX, IDC_DLG_LIST, m_CmdList);
}

BEGIN_MESSAGE_MAP(CPfwDlg, CDialog)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
ON_NOTIFY(NM_CLICK, IDC_DLG_LIST, OnNMClickDlgList)
ON_COMMAND(ID_CMD_EXIT, OnCmdExit)
ON_COMMAND(ID_CMD_RESTORE, OnCmdRestore)
ON_COMMAND(ID_CMD_ABOUT, OnCmdAbout)
ON_COMMAND(ID_CMD_FILTERDLG, OnCmdFilterdlg)
ON_COMMAND(ID_CMD_APPDLG, OnCmdAppdlg)
ON_COMMAND(ID_CMD_CONFIG, OnCmdConfig)
ON_COMMAND(ID_CMD_LOGINFO, OnCmdLoginfo)
ON_MESSAGE( WM_NOTIFYICON, OnNotifyIcon)
ON_MESSAGE( WM_PFW_NOTIFY, OnPfwNotify )
ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPfwDlg ��Ϣ�������

BOOL CPfwDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	NOTIFYICONDATA nid;
	nid.cbSize				= sizeof(nid);
	nid.hWnd				= m_hWnd;
	nid.uID					= ID_NOTIFYICON;
	nid.uFlags				= NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage	= WM_NOTIFYICON;
	nid.hIcon				= m_hIcon;
	GetWindowText( nid.szTip, sizeof(nid.szTip)/sizeof(TCHAR) );

	Shell_NotifyIcon( NIM_ADD, &nid );

	m_NotifyMenu.LoadMenu( IDR_NOTIFYMENU );

	InitCmdList();
	InitDlgCtrl();

	PfwSetNotifiedWnd( m_hWnd );
	PfwEnable( TRUE );

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}
void CPfwDlg::OnDestroy()
{
	PfwEnable( FALSE );
	PfwSetNotifiedWnd( NULL );

	CDialog::OnDestroy();

	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = m_idNotifyIcon;

	Shell_NotifyIcon( NIM_DELETE, &nid );
}

/////////////////////////////////////////////////////////////////////

void CPfwDlg::InitCmdList()
{
	m_ImageList.Create( 32, 32, ILC_COLOR8, 0, 1 );
	m_ImageList.SetBkColor( m_CmdList.GetBkColor() );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_LOG_DLG) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_FILTER_DLG) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_APP_DLG) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_CONFIG_DLG) );
	m_ImageList.Add( AfxGetApp()->LoadIcon(IDI_ABOUT_DLG) );

	m_CmdList.SetImageList( &m_ImageList, LVSIL_NORMAL );

	CString szItem;
	int i= 0;

	szItem.LoadString(IDS_LOGINFO);
	m_CmdList.InsertItem( i, szItem, i );
	m_CmdList.SetItemData( i, ID_CMD_LOGINFO );
	i++;
	szItem.LoadString(IDS_FILTERDLG);
	m_CmdList.InsertItem( i, szItem, i );
	m_CmdList.SetItemData( i, ID_CMD_FILTERDLG );
	i++;
	szItem.LoadString(IDS_APPDLG);
	m_CmdList.InsertItem( i, szItem, i );
	m_CmdList.SetItemData( i, ID_CMD_APPDLG );
	i++;
	szItem.LoadString(IDS_CONFIG);
	m_CmdList.InsertItem( i, szItem, i );
	m_CmdList.SetItemData( i, ID_CMD_CONFIG );
	i++;
	szItem.LoadString(IDS_ABOUT);
	m_CmdList.InsertItem( i, szItem, i );
	m_CmdList.SetItemData( i, ID_CMD_ABOUT );
}
void CPfwDlg::InitDlgCtrl()
{
	m_DlgCtrl.SetDlg( &m_dlgFilter, m_dlgFilter.IDD );
	m_DlgCtrl.SetDlg( &m_dlgApp, m_dlgApp.IDD );
	m_DlgCtrl.SetDlg( &m_dlgConfig, m_dlgConfig.IDD );
	m_DlgCtrl.SetDlg( &m_dlgLog, m_dlgLog.IDD );
}

void CPfwDlg::NotifyIconInfo( LPCTSTR szInfo )
{
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize		= sizeof(NOTIFYICONDATA);
	nid.hWnd		= m_hWnd;
	nid.uID			= ID_NOTIFYICON;
	nid.uFlags		= NIF_INFO;
	nid.uTimeout	= 10000;
	nid.dwInfoFlags	= NIIF_INFO;
	memcpy( nid.szInfo, szInfo, sizeof(nid.szInfo) );
	GetWindowText( nid.szInfoTitle, sizeof(nid.szInfoTitle)/sizeof(TCHAR) );

    Shell_NotifyIcon( NIM_MODIFY, &nid );
}

//////////////////////////////////////////////////////////////////////
//��Ϣ������


void CPfwDlg::OnNMClickDlgList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_CmdList.GetFirstSelectedItemPosition();
	int nItem = m_CmdList.GetNextSelectedItem( pos );
	if ( nItem >= 0 && nItem < m_CmdList.GetItemCount() )
	{
		WORD wID = (WORD)m_CmdList.GetItemData( nItem );
		if ( wID != 0 )
			SendMessage( WM_COMMAND, MAKEWPARAM(wID,0), 0 );
	}
    *pResult = 0;
}

void CPfwDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch ( nID )
	{
	case SC_MINIMIZE:
		CDialog::OnSysCommand(nID, lParam);
		ShowWindow( SW_HIDE );
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

LRESULT CPfwDlg::OnNotifyIcon( WPARAM wParam, LPARAM lParam )
{
	ASSERT( (UINT)wParam == m_idNotifyIcon );
	UINT uMouseMsg = (UINT)lParam;

	switch (uMouseMsg)
	{
	case WM_LBUTTONDOWN:
		OnCmdRestore();
		break;
	case WM_RBUTTONDOWN:
		POINT pt;
		GetCursorPos( &pt );
		SetForegroundWindow();
		CMenu *sub;
		sub = m_NotifyMenu.GetSubMenu( 0 );
		sub->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pt.x,pt.y,this);
		PostMessage( WM_NULL );
		break;
	} 
	return 0;
}
//���� PFW �豸�����ӿڷ�����֪ͨ��Ϣ
LRESULT CPfwDlg::OnPfwNotify( WPARAM wParam, LPARAM lParam)
{
	if ( wParam == NT_TDI_REQUEST )
	{
		HANDLE pid = (HANDLE) lParam;
		TCHAR szFileName[MAX_PATH];

		if ( GetProcessFileName( pid, szFileName, MAX_PATH )
			&& IsWindow(m_dlgApp.m_hWnd) )
		{
			m_dlgApp.OnAppRuleInsert( CARule(szFileName) );
		}
	}
	else if ( wParam == NT_LOG )
	{
		LOG_INFO *pLog = (LOG_INFO*)lParam;

		if ( NULL != pLog )
		{
			if ( (pLog->action & ACTION_RECORD) 
					&& IsWindow(m_dlgLog.m_hWnd) )
			{
				m_dlgLog.OnInsertLog( *pLog );
			}
			if ( pLog->action & ACTION_FLASH )
			{
				//TODO: ϵͳ��ͼ��������ʾ
				TCHAR szFileName[MAX_PATH];
				GetProcessFileName( pLog->pid, szFileName, MAX_PATH );
				TCHAR szFullName[MAX_PATH];
				TCHAR *lpszName;
				GetFullPathName( szFileName, MAX_PATH, szFullName, &lpszName );

				UINT uID = IS_ACTION_PASS(pLog->action) ? 
						IDS_FORMAT_TRAYINFO_PASS : IDS_FORMAT_TRAYINFO_DENY;
				CString szInfo;
				CTime time = CTime::GetCurrentTime();
				szInfo.Format( uID, time.Format(_T("%H:%M:%S")), lpszName );
				NotifyIconInfo( szInfo );
			}
			if ( pLog->action & ACTION_BEEP )
			{
				//MessageBeep( -1 );
				TCHAR szAppPath[MAX_PATH];
				CString szFileName;

				theApp.GetAppPath( szAppPath, MAX_PATH );
				szFileName.LoadString( IDS_FN_BEEP );
				szFileName = szAppPath + szFileName;
				sndPlaySound( szFileName, SND_ASYNC );
			}
		}
	}
	return 0;
}

void CPfwDlg::OnOK()
{
}

void CPfwDlg::OnCancel()
{
	SendMessage( WM_SYSCOMMAND, SC_MINIMIZE, 0 );
}

void CPfwDlg::OnCmdExit()
{
	CDialog::OnOK();
}

void CPfwDlg::OnCmdRestore()
{
	ShowWindow( SW_RESTORE );
	SetForegroundWindow();
}

void CPfwDlg::OnCmdAbout()
{
	CDialog dlgAbout(IDD_ABOUTBOX,this);
	dlgAbout.DoModal();
}

void CPfwDlg::OnCmdFilterdlg()
{
	m_DlgCtrl.SetDlg( &m_dlgFilter, m_dlgFilter.IDD );
}

void CPfwDlg::OnCmdAppdlg()
{
	m_DlgCtrl.SetDlg( &m_dlgApp, m_dlgApp.IDD );
}

void CPfwDlg::OnCmdConfig()
{
	m_DlgCtrl.SetDlg( &m_dlgConfig, m_dlgConfig.IDD );
}

void CPfwDlg::OnCmdLoginfo()
{
	m_DlgCtrl.SetDlg( &m_dlgLog, m_dlgLog.IDD );
}


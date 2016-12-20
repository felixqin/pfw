// PfwDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "DlgCtrl.h"
#include "FilterDlg.h"
#include "AppDlg.h"
#include "LogDlg.h"
#include "ConfigDlg.h"
#include "afxwin.h"


//窗口用来接受系统盒事件的消息 ID
#define ID_NOTIFYICON	567
#define WM_NOTIFYICON	WM_USER + ID_NOTIFYICON

// CPfwDlg 对话框
class CPfwDlg : public CDialog
{
// 构造
public:
	CPfwDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	UINT m_idNotifyIcon;
	CMenu m_NotifyMenu;

protected:
	void InitCmdList();
	void InitDlgCtrl();
	void NotifyIconInfo( LPCTSTR szTips );

public:
	CDlgCtrl		m_DlgCtrl;
	CListCtrl		m_CmdList;
	CImageList		m_ImageList;
	CFilterDlg		m_dlgFilter;
	CAppDlg			m_dlgApp;
	CLogDlg			m_dlgLog;
	CConfigDlg		m_dlgConfig;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnCmdExit();
	afx_msg void OnCmdRestore();
	afx_msg LRESULT OnNotifyIcon( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPfwNotify( WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnNMClickDlgList(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnCmdAbout();
	afx_msg void OnCmdFilterdlg();
	afx_msg void OnCmdAppdlg();
	afx_msg void OnCmdConfig();
	afx_msg void OnCmdLoginfo();
};

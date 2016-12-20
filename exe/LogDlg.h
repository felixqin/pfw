#pragma once
#include "afxcmn.h"


// CLogDlg 对话框

class CLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogDlg();

// 对话框数据
	enum { IDD = IDD_LOG_DIALOG };

private:
	CListCtrl m_LogList;
	CImageList m_ImageList;

	BOOL m_bScroll;		//是否滚动
	BOOL m_bLogEnable;	//是否记录

protected:
	void InitLogList();
	void InsertLogToList( const LOG_INFO &log );
	int SaveLogList( LPCTSTR lpszFileName );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnInsertLog( const LOG_INFO &log );
	afx_msg void OnCmdLogEnable();
	afx_msg void OnCmdScrollLog();
	afx_msg void OnCmdClearLog();
	afx_msg void OnCmdSaveLog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};

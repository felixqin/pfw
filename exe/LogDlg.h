#pragma once
#include "afxcmn.h"


// CLogDlg �Ի���

class CLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogDlg();

// �Ի�������
	enum { IDD = IDD_LOG_DIALOG };

private:
	CListCtrl m_LogList;
	CImageList m_ImageList;

	BOOL m_bScroll;		//�Ƿ����
	BOOL m_bLogEnable;	//�Ƿ��¼

protected:
	void InitLogList();
	void InsertLogToList( const LOG_INFO &log );
	int SaveLogList( LPCTSTR lpszFileName );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

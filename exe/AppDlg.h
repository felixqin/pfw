#pragma once
#include "afxcmn.h"


// CAppDlg �Ի���

class CAppDlg : public CDialog
{
	DECLARE_DYNAMIC(CAppDlg)

public:
	CAppDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAppDlg();

// �Ի�������
	enum { IDD = IDD_APP_DIALOG };

public:
	CListCtrl m_AppList;
	CImageList m_AppImage;

	CAppRules* const m_pAppRules;

public:
	void InitAppList();
	void InsertListItem( LPCTSTR szFileName );
	void OnAppRuleInsert(const CARule &r);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCmdModify();

	afx_msg void OnCmdDelete();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnNMDblclkAppList(NMHDR *pNMHDR, LRESULT *pResult);
};

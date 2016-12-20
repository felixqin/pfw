#pragma once
#include "afxcmn.h"


// CFilterDlg �Ի���

class CFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CFilterDlg)

public:
	CFilterDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFilterDlg();

// �Ի�������
	enum { IDD = IDD_FILTER_DIALOG };

protected:
	CListCtrl m_FilterList;
	CImageList m_ImageList;

	CFilterRules *m_pFilterRules;

protected:
	void InitFilterList();
	void InsertListItem( int nItem, USHORT, LPCTSTR, LPCTSTR );
	void UpdateListItem( int nItem, USHORT, LPCTSTR, LPCTSTR );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCmdInsert();
	afx_msg void OnCmdDelete();
	afx_msg void OnCmdModify();
	afx_msg void OnCmdMoveUp();
	afx_msg void OnCmdMoveDown();
	afx_msg void OnNMDblclkFilterList(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
	virtual void OnCancel();
};

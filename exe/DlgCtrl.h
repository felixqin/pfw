#pragma once


// CDlgCtrl

class CDlgCtrl : public CStatic
{
	DECLARE_DYNAMIC(CDlgCtrl)

public:
	CDlgCtrl();
	virtual ~CDlgCtrl();
public:
	BOOL SetDlg(CDialog * pDlg, UINT nIDTemplate);
	void DestroyAllDlg();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CDialog *m_pDlg;	//当前对话框
public:
	afx_msg void OnDestroy();
};



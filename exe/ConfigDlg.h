#pragma once


// CConfigDlg �Ի���

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigDlg();

// �Ի�������
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	BOOL m_bAutoStart;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCmdApply();
	afx_msg void OnCmdRestore();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};

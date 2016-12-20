#pragma once


// CFRuleInfoDlg 对话框

class CFRuleInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CFRuleInfoDlg)

public:
	CFRuleInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFRuleInfoDlg();

// 对话框数据
	enum { IDD = IDD_FILTERRULE_INFO };
private:
	CString m_szName;
	CString m_szDescription;
	int m_nProtoType;
	BOOL m_bDirectIn;
	int m_nAddrType;
	DWORD m_dwIP;
	DWORD m_dwMask;
	UINT m_uPort1;
	UINT m_uPort2;
	int m_nActionType;
	BOOL m_bRecord;
	BOOL m_bFlash;
	BOOL m_bBeep;

public:
	CFRule m_Rule;

private:
	void InitDlgItem();

	void RuleToData();
	void DataToRule();
	void UpdateDlgItem();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnNeedUpdateDlgItem();
};

#pragma once

#include "AppRule.h"
#include "afxwin.h"


// CARuleInfoDlg 对话框

class CARuleInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CARuleInfoDlg)

public:
	CARuleInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CARuleInfoDlg();

// 对话框数据
	enum { IDD = IDD_APPRULE_INFO };

public:
	CARule m_Rule;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString m_szAppName;

	BOOL m_bTcpin;
	BOOL m_bTcpout;
	BOOL m_bUdpin;
	BOOL m_bUdpout;
	BOOL m_bLog;
	BOOL m_bFlash;
	BOOL m_bBeep;

	BOOL m_bTcpPortsList;
	BOOL m_bUdpPortsList;
	UINT m_uTcpPort1;
	UINT m_uTcpPort2;
	UINT m_uUdpPort1;
	UINT m_uUdpPort2;
	CEdit m_TcpPorts;
	CString m_szTcpPorts;
	CString m_szUdpPorts;
protected:
	void PortsToString( CString &szPort, const USHORT ports[], int count );
	int StringToPorts( LPCTSTR szPorts, USHORT ports[], int max_count );
	void RuleToData();
	void DataToRule();
	void UpdateDlgItem();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickeProtoTypeRadio();
	virtual void OnOK();
	CStatic m_AppIcon;
	afx_msg void OnDestroy();
};

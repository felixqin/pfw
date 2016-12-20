// FRuleInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "exe.h"
#include "FRuleInfoDlg.h"
#include ".\fruleinfodlg.h"

const TCHAR szDefRuleName[] = _T("<输入规则名>");
const TCHAR szDefRuleDesc[] = _T("<输入规则说明>");

// CFRuleInfoDlg 对话框

IMPLEMENT_DYNAMIC(CFRuleInfoDlg, CDialog)
CFRuleInfoDlg::CFRuleInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFRuleInfoDlg::IDD, pParent)
	, m_Rule(szDefRuleName,szDefRuleDesc)
	, m_szName(_T(""))
	, m_szDescription(_T(""))
	, m_nProtoType(0)
	, m_bDirectIn(FALSE)
	, m_nAddrType(0)
	, m_dwIP(0)
	, m_dwMask(0)
	, m_uPort1(0)
	, m_uPort2(0)
	, m_nActionType(0)
	, m_bRecord(FALSE)
	, m_bFlash(FALSE)
	, m_bBeep(FALSE)
{
}

CFRuleInfoDlg::~CFRuleInfoDlg()
{
}

void CFRuleInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_szName);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_szDescription);
	DDX_Radio(pDX, IDC_PROTO_TCP, m_nProtoType);
	DDX_Radio(pDX, IDC_DIRECT_OUT, m_bDirectIn);
	DDX_CBIndex(pDX, IDC_COMBO_IP_TYPE, m_nAddrType);
	DDX_IPAddress(pDX, IDC_IPADDRESS_IP, m_dwIP);
	DDX_IPAddress(pDX, IDC_IPADDRESS_MASK, m_dwMask);
	DDX_Text(pDX, IDC_EDIT_PORT1, m_uPort1);
	DDV_MinMaxUInt(pDX, m_uPort1, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_PORT2, m_uPort2);
	DDV_MinMaxUInt(pDX, m_uPort2, 0, 65535);
	DDX_CBIndex(pDX, IDC_COMBO_ACTION, m_nActionType);
	DDX_Check(pDX, IDC_CHECK_RECORD, m_bRecord);
	DDX_Check(pDX, IDC_CHECK_FLASH, m_bFlash);
	DDX_Check(pDX, IDC_CHECK_BEEP, m_bBeep);
}


BEGIN_MESSAGE_MAP(CFRuleInfoDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_IP_TYPE, OnNeedUpdateDlgItem)
	ON_BN_CLICKED(IDC_PROTO_IP, OnNeedUpdateDlgItem)
	ON_BN_CLICKED(IDC_PROTO_TCP, OnNeedUpdateDlgItem)
	ON_BN_CLICKED(IDC_PROTO_UDP, OnNeedUpdateDlgItem)
	ON_BN_CLICKED(IDC_DIRECT_IN, OnNeedUpdateDlgItem)
	ON_BN_CLICKED(IDC_DIRECT_OUT, OnNeedUpdateDlgItem)
END_MESSAGE_MAP()


BOOL CFRuleInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitDlgItem();
	RuleToData();
	UpdateDlgItem();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CFRuleInfoDlg::InitDlgItem()
{
	CString txt;
	CComboBox *pIpType = (CComboBox*)GetDlgItem( IDC_COMBO_IP_TYPE );
	CComboBox *pActionType = (CComboBox*)GetDlgItem( IDC_COMBO_ACTION );

	txt.LoadString( IDS_ADDR_TYPE_ANY );
	pIpType->AddString( txt );
	txt.LoadString( IDS_ADDR_TYPE_LAN );
	pIpType->AddString( txt );
	txt.LoadString( IDS_ADDR_TYPE_SPEC );
	pIpType->AddString( txt );
	txt.LoadString( IDS_ADDR_TYPE_NETWORK );
	pIpType->AddString( txt );

	txt.LoadString( IDS_ACTION_PASS );
	pActionType->AddString( txt );
	txt.LoadString( IDS_ACTION_DENY );
	pActionType->AddString( txt );
	txt.LoadString( IDS_ACTION_CONTINUE_EX );
	pActionType->AddString( txt );
}
void CFRuleInfoDlg::RuleToData()
{
	const FILTER_RULE *rule = m_Rule.GetRule();
	m_szName = rule->name;
	m_szDescription = rule->description;
	//协议类型
	if ( rule->type & RT_TCP )
	{
		m_nProtoType = 0;
	}
	else if ( rule->type & RT_UDP )
	{
		m_nProtoType = 1;
	}
	else
	{
		m_nProtoType = 2;
	}
	//数据方向
	m_bDirectIn = !IS_DIRECTOUT( rule->type );
	//地址类型
	if ( rule->ip == 0 && rule->mask == 0 )
	{
		m_nAddrType = 0;	//任何地址
	}
	else if ( (rule->ip & rule->mask) == 0xC0A80000 )//地址是 192.168.x.x
	{
		m_nAddrType = 1;	//局域网地址
	}
	else if ( rule->mask == 0xFFFFFFFF )
	{
		m_nAddrType = 2;	//指定地址
	}
	else
	{
		m_nAddrType = 3;	//网络地址
	}
	//IP 地址 和 掩码
	m_dwIP = rule->ip;
	m_dwMask = rule->mask;
	//起止端口
	m_uPort1 = rule->port1;
	m_uPort2 = rule->port2;
	//动作和记录
	if ( IS_ACTION_CONTINUE(rule->action) )
	{
		m_nActionType = 2;
	}
	else if ( IS_ACTION_PASS(rule->action) )
	{
		m_nActionType = 0;
	}
	else
	{
		m_nActionType = 1;
	}
	m_bRecord	= (rule->action & ACTION_RECORD) != 0;
	m_bFlash	= (rule->action & ACTION_FLASH) != 0;
	m_bBeep		= (rule->action & ACTION_BEEP) != 0;
}
void CFRuleInfoDlg::DataToRule()
{
	//协议类型
	USHORT type;
	if ( m_nProtoType == 0 )
	{
		type = RT_TCP;
	}
	else if ( m_nProtoType == 1 )
	{
		type = RT_UDP;
	}
	else
	{
		type = RT_IP;
	}
	//数据方向
	type |= m_bDirectIn ? RT_DIRECTIN : RT_DIRECTOUT;
	
	//地址类型//IP 地址 和 掩码
	ULONG ip,mask;
	if ( m_nAddrType == 0 )	//任何地址
	{
		ip = 0;
		mask = 0;
	}
	else if ( m_nAddrType == 1 )	//局域网地址
	{
		ip = 0xC0A80000;	//地址是 192.168.x.x
		mask = 0xFFFF0000;
	}
	else if ( m_nAddrType == 2 )	//指定地址
	{
		ip = m_dwIP;
		mask = 0xFFFFFFFF;
	}
	else	//网络地址
	{
		ip = m_dwIP;
		mask = m_dwMask;
	}
	//起止端口
	USHORT port1 = m_uPort1;
	USHORT port2 = m_uPort2;
	//动作和记录
	USHORT action;
	if ( m_nActionType == 2 )
	{
		action = ACTION_CONTINUE;
	}
	else if ( m_nActionType == 0 )
	{
		action = ACTION_PASS;
	}
	else
	{
		action = ACTION_DENY;
	}
	if (m_bRecord) action |= ACTION_RECORD;
	if (m_bFlash) action |= ACTION_FLASH;
	if (m_bBeep) action |= ACTION_BEEP;

	m_Rule = CFRule( m_szName, m_szDescription, type, 
					ip, mask, port1, port2, action );
}
void CFRuleInfoDlg::UpdateDlgItem()
{
	UpdateData( FALSE );
	//隐藏地址设置
	GetDlgItem(IDC_STATIC_IP)->ShowWindow( m_nAddrType > 1 );
	GetDlgItem(IDC_IPADDRESS_IP)->ShowWindow( m_nAddrType > 1 );
	GetDlgItem(IDC_STATIC_MASK)->ShowWindow( m_nAddrType == 3 );
	GetDlgItem(IDC_IPADDRESS_MASK)->ShowWindow( m_nAddrType == 3 );
	//设置端口范围标签
	UINT uID = m_bDirectIn ? IDS_PORT_RANG_LOCAL : IDS_PORT_RANG_REMOTE;
	CString txt;	txt.LoadString( uID );
	SetDlgItemText( IDC_PORT_LABEL, txt );
	//隐藏端口设置
	GetDlgItem(IDC_EDIT_PORT1)->EnableWindow( m_nProtoType != 2 );
	GetDlgItem(IDC_EDIT_PORT2)->EnableWindow( m_nProtoType != 2 );
}

// CFRuleInfoDlg 消息处理程序

void CFRuleInfoDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();
	DataToRule();

	CDialog::OnOK();
}

void CFRuleInfoDlg::OnNeedUpdateDlgItem()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	UpdateDlgItem();
}

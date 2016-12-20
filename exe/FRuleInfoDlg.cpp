// FRuleInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "exe.h"
#include "FRuleInfoDlg.h"
#include ".\fruleinfodlg.h"

const TCHAR szDefRuleName[] = _T("<���������>");
const TCHAR szDefRuleDesc[] = _T("<�������˵��>");

// CFRuleInfoDlg �Ի���

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitDlgItem();
	RuleToData();
	UpdateDlgItem();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
	//Э������
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
	//���ݷ���
	m_bDirectIn = !IS_DIRECTOUT( rule->type );
	//��ַ����
	if ( rule->ip == 0 && rule->mask == 0 )
	{
		m_nAddrType = 0;	//�κε�ַ
	}
	else if ( (rule->ip & rule->mask) == 0xC0A80000 )//��ַ�� 192.168.x.x
	{
		m_nAddrType = 1;	//��������ַ
	}
	else if ( rule->mask == 0xFFFFFFFF )
	{
		m_nAddrType = 2;	//ָ����ַ
	}
	else
	{
		m_nAddrType = 3;	//�����ַ
	}
	//IP ��ַ �� ����
	m_dwIP = rule->ip;
	m_dwMask = rule->mask;
	//��ֹ�˿�
	m_uPort1 = rule->port1;
	m_uPort2 = rule->port2;
	//�����ͼ�¼
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
	//Э������
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
	//���ݷ���
	type |= m_bDirectIn ? RT_DIRECTIN : RT_DIRECTOUT;
	
	//��ַ����//IP ��ַ �� ����
	ULONG ip,mask;
	if ( m_nAddrType == 0 )	//�κε�ַ
	{
		ip = 0;
		mask = 0;
	}
	else if ( m_nAddrType == 1 )	//��������ַ
	{
		ip = 0xC0A80000;	//��ַ�� 192.168.x.x
		mask = 0xFFFF0000;
	}
	else if ( m_nAddrType == 2 )	//ָ����ַ
	{
		ip = m_dwIP;
		mask = 0xFFFFFFFF;
	}
	else	//�����ַ
	{
		ip = m_dwIP;
		mask = m_dwMask;
	}
	//��ֹ�˿�
	USHORT port1 = m_uPort1;
	USHORT port2 = m_uPort2;
	//�����ͼ�¼
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
	//���ص�ַ����
	GetDlgItem(IDC_STATIC_IP)->ShowWindow( m_nAddrType > 1 );
	GetDlgItem(IDC_IPADDRESS_IP)->ShowWindow( m_nAddrType > 1 );
	GetDlgItem(IDC_STATIC_MASK)->ShowWindow( m_nAddrType == 3 );
	GetDlgItem(IDC_IPADDRESS_MASK)->ShowWindow( m_nAddrType == 3 );
	//���ö˿ڷ�Χ��ǩ
	UINT uID = m_bDirectIn ? IDS_PORT_RANG_LOCAL : IDS_PORT_RANG_REMOTE;
	CString txt;	txt.LoadString( uID );
	SetDlgItemText( IDC_PORT_LABEL, txt );
	//���ض˿�����
	GetDlgItem(IDC_EDIT_PORT1)->EnableWindow( m_nProtoType != 2 );
	GetDlgItem(IDC_EDIT_PORT2)->EnableWindow( m_nProtoType != 2 );
}

// CFRuleInfoDlg ��Ϣ�������

void CFRuleInfoDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	UpdateData();
	DataToRule();

	CDialog::OnOK();
}

void CFRuleInfoDlg::OnNeedUpdateDlgItem()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	UpdateDlgItem();
}

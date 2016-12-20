// ARuleInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "exe.h"
#include "ARuleInfoDlg.h"
#include ".\aruleinfodlg.h"


// CARuleInfoDlg 对话框

IMPLEMENT_DYNAMIC(CARuleInfoDlg, CDialog)
CARuleInfoDlg::CARuleInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARuleInfoDlg::IDD, pParent)
	, m_Rule( _T("") ) //规则初始化
	, m_bTcpin(FALSE)
	, m_bTcpout(FALSE)
	, m_bUdpin(FALSE)
	, m_bUdpout(FALSE)
	, m_bLog(FALSE)
	, m_bFlash(FALSE)
	, m_bBeep(FALSE)
	, m_bTcpPortsList(FALSE)
	, m_bUdpPortsList(FALSE)
	, m_uTcpPort1(0)
	, m_uTcpPort2(0)
	, m_uUdpPort1(0)
	, m_uUdpPort2(0)
	, m_szAppName(_T(""))
	, m_szTcpPorts(_T(""))
	, m_szUdpPorts(_T(""))
{
}

CARuleInfoDlg::~CARuleInfoDlg()
{
}

void CARuleInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_TCPIN, m_bTcpin);
	DDX_Check(pDX, IDC_CHECK_TCPOUT, m_bTcpout);
	DDX_Check(pDX, IDC_CHECK_UDPIN, m_bUdpin);
	DDX_Check(pDX, IDC_CHECK_UDPOUT, m_bUdpout);
	DDX_Check(pDX, IDC_CHECK_LOG, m_bLog);
	DDX_Check(pDX, IDC_CHECK_FLASH, m_bFlash);
	DDX_Check(pDX, IDC_CHECK_BEEP, m_bBeep);
	DDX_Radio(pDX, IDC_RADIO_TCP_RANGE, m_bTcpPortsList);
	DDX_Radio(pDX, IDC_RADIO_UDP_RANGE, m_bUdpPortsList);
	DDX_Text(pDX, IDC_EDIT_TCP_PORT1, m_uTcpPort1);
	DDV_MinMaxUInt(pDX, m_uTcpPort1, 0, 0xFFFF);
	DDX_Text(pDX, IDC_EDIT_TCP_PORT2, m_uTcpPort2);
	DDV_MinMaxUInt(pDX, m_uTcpPort2, 0, 0xFFFF);
	DDX_Text(pDX, IDC_EDIT_UDP_PORT1, m_uUdpPort1);
	DDV_MinMaxUInt(pDX, m_uUdpPort1, 0, 0xFFFF);
	DDX_Text(pDX, IDC_EDIT_UDP_PORT2, m_uUdpPort2);
	DDV_MinMaxUInt(pDX, m_uUdpPort2, 0, 0xFFFF);
	DDX_Text(pDX, IDC_APPNAME, m_szAppName);
	DDX_Text(pDX, IDC_EDIT_TCP_PORTS, m_szTcpPorts);
	DDX_Text(pDX, IDC_EDIT_UDP_PORTS, m_szUdpPorts);
	DDX_Control(pDX, IDC_APPICON, m_AppIcon);
}


BEGIN_MESSAGE_MAP(CARuleInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_TCP_RANGE, OnBnClickeProtoTypeRadio)
	ON_BN_CLICKED(IDC_RADIO_TCP_LIST, OnBnClickeProtoTypeRadio)
	ON_BN_CLICKED(IDC_RADIO_UDP_RANGE, OnBnClickeProtoTypeRadio)
	ON_BN_CLICKED(IDC_RADIO_UDP_LIST, OnBnClickeProtoTypeRadio)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CARuleInfoDlg 消息处理程序

BOOL CARuleInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	RuleToData();
	UpdateData( FALSE );
	UpdateDlgItem();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CARuleInfoDlg::PortsToString( CString &szPorts, const USHORT ports[], int count )
{
	szPorts.Empty();
	for ( int i=0; i<count; i++ )
	{
		CString szTemp;
		szTemp.Format( _T("%d\r\n"), ports[i] );
		szPorts += szTemp;
	}
}
int CARuleInfoDlg::StringToPorts( LPCTSTR szPorts, USHORT ports[], int max_count )
{
	TCHAR line[20];
	int cc = 0;
	TCHAR *p = (TCHAR*)szPorts;
	while ( *p != _T('\0') )
	{
		TCHAR *ln = line;
		while ( *p != _T('\r') && *p != _T('\n') && *p != _T('\0') )
		{
			if ( *p < _T('0') || *p > _T('9') )
			{
				return 0;	//出现非法字符
			}
			*ln++ = *p++;
		}
		*ln = _T('\0');
		ports[cc++] = _tstoi(line);

		while ( *p == _T('\r') || *p == _T('\n') ) p++;	//跳过换行符
	}
	return cc;
}
void CARuleInfoDlg::RuleToData()
{
	//程序名
	const APP_RULE *pRule = m_Rule.GetRule();
	m_szAppName = pRule->app_name;

	//允许的协议类型
	m_bTcpin = (pRule->allow & ALLOW_TCP_IN) != 0;
	m_bTcpout = (pRule->allow & ALLOW_TCP_OUT) != 0;
	m_bUdpin = (pRule->allow & ALLOW_UDP_IN) != 0;
	m_bUdpout = (pRule->allow & ALLOW_UDP_OUT) != 0;

	//可访问的端口
	m_bTcpPortsList = (pRule->pt_tcp == PT_LIST);
	m_bUdpPortsList = (pRule->pt_udp == PT_LIST);

	const USHORT *tcp_ports = &pRule->ports[0];
	const USHORT *udp_ports = &pRule->ports[pRule->pn_tcp];

	//端口列表处理
	if ( !m_bTcpPortsList )
	{
		m_uTcpPort1 = tcp_ports[0];
		m_uTcpPort2 = tcp_ports[1];
	}
	else
	{
		PortsToString(m_szTcpPorts,tcp_ports,pRule->pn_tcp);
	}
	if ( !m_bUdpPortsList )
	{
		m_uUdpPort1 = udp_ports[0];
		m_uUdpPort2 = udp_ports[1];
	}
	else
	{
		PortsToString(m_szUdpPorts,udp_ports,pRule->pn_udp);
		//MessageBox(m_szUdpPorts);
	}

	//过滤动作
	m_bLog = (pRule->action & ACTION_RECORD) != 0;
	m_bFlash = (pRule->action & ACTION_FLASH) != 0;
	m_bBeep = (pRule->action & ACTION_BEEP) != 0;
}
void CARuleInfoDlg::DataToRule()
{
	USHORT allow = 0;		//可用协议类型
	if (m_bTcpin) allow |= ALLOW_TCP_IN;
	if (m_bTcpout) allow |= ALLOW_TCP_OUT;
	if (m_bUdpin) allow |= ALLOW_UDP_IN;
	if (m_bUdpout) allow |= ALLOW_UDP_OUT;

	USHORT action = ACTION_DENY;	//过滤动作
	if (m_bLog) action |= ACTION_RECORD;
	if (m_bFlash) action |= ACTION_FLASH;
	if (m_bBeep) action |= ACTION_BEEP;

	//端口表类型
	USHORT pt_tcp = m_bTcpPortsList ? PT_LIST : PT_RANGE;
	USHORT pt_udp = m_bUdpPortsList ? PT_LIST : PT_RANGE;

	//可访问端口处理
	const int max_num = 100;
	USHORT tcp_ports[max_num];
	USHORT udp_ports[max_num];
	int pn_tcp, pn_udp;

	if ( m_bTcpPortsList )
	{
		pn_tcp = StringToPorts( m_szTcpPorts, tcp_ports, max_num );
	}
	else
	{
		pn_tcp = 2;
		tcp_ports[0] = m_uTcpPort1;
		tcp_ports[1] = m_uTcpPort2;
	}
	if ( m_bUdpPortsList )
	{
		//MessageBox(m_szUdpPorts);
		pn_udp = StringToPorts( m_szUdpPorts, udp_ports, max_num );
	}
	else
	{
		pn_udp = 2;
		udp_ports[0] = m_uUdpPort1;
		udp_ports[1] = m_uUdpPort2;
	}

	if ( pn_tcp == 0 || pn_udp == 0 )
	{
		AfxMessageBox( IDS_MB_ERROR_INPUT, MB_ICONERROR );
		return;
	}

	//修改原规则
	m_Rule = CARule( m_Rule.GetRule()->app_name, allow, action, 
					pt_tcp, pn_tcp, tcp_ports,
					pt_udp, pn_udp, udp_ports );
}
void CARuleInfoDlg::UpdateDlgItem()
{
	ASSERT( m_szAppName != _T("") );

	//图标处理
	SHFILEINFO    sfi;
	SHGetFileInfo(m_szAppName,0,&sfi,sizeof(SHFILEINFO),SHGFI_ICON);
	m_AppIcon.SetIcon( sfi.hIcon );

	GetDlgItem(IDC_EDIT_TCP_PORT1)->EnableWindow( !m_bTcpPortsList );
	GetDlgItem(IDC_EDIT_TCP_PORT2)->EnableWindow( !m_bTcpPortsList );
	GetDlgItem(IDC_EDIT_TCP_PORTS)->EnableWindow( m_bTcpPortsList );
	GetDlgItem(IDC_EDIT_UDP_PORT1)->EnableWindow( !m_bUdpPortsList );
	GetDlgItem(IDC_EDIT_UDP_PORT2)->EnableWindow( !m_bUdpPortsList );
	GetDlgItem(IDC_EDIT_UDP_PORTS)->EnableWindow( m_bUdpPortsList );
}

void CARuleInfoDlg::OnBnClickeProtoTypeRadio()
{
	UpdateData( TRUE );
	UpdateDlgItem();
}

void CARuleInfoDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();
	DataToRule();

	CDialog::OnOK();
}

void CARuleInfoDlg::OnDestroy()
{
	CDialog::OnDestroy();

	//释放图标资源
	DestroyIcon( m_AppIcon.GetIcon() );
}

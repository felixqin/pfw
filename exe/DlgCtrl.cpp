// DlgCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgCtrl.h"


// CDlgCtrl

IMPLEMENT_DYNAMIC(CDlgCtrl, CStatic)
CDlgCtrl::CDlgCtrl()
{
	m_pDlg=NULL;
}

CDlgCtrl::~CDlgCtrl()
{
}


BEGIN_MESSAGE_MAP(CDlgCtrl, CStatic)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CDlgCtrl 消息处理程序


BOOL CDlgCtrl::SetDlg(CDialog * pDlg, UINT nIDTemplate)
{
	ASSERT(pDlg!=NULL);

	//是当前窗口且已经显示
	if (pDlg == m_pDlg)
	{
		return TRUE;
	}
	//pDlg 不是窗口且创建不成功
	if (!IsWindow(pDlg->m_hWnd) &&
			!pDlg->Create(nIDTemplate,this))
	{
		return FALSE;
	}

	//注意: 请保证对话框模板 Style 的值为"子级"
	ASSERT(IsChild(pDlg));

	//对话框显示切换
	pDlg->ShowWindow(SW_SHOW);
	if (m_pDlg != NULL)
	{
		m_pDlg->ShowWindow(SW_HIDE);
	}
	m_pDlg = pDlg;

	return TRUE;
}

void CDlgCtrl::DestroyAllDlg()
{
	ASSERT(IsWindow(m_hWnd));

	CWnd *pWnd;
	while (NULL != (pWnd = GetWindow(GW_CHILD)))
	{
		pWnd->DestroyWindow();
	}
}

void CDlgCtrl::OnDestroy()
{
	DestroyAllDlg();
	CStatic::OnDestroy();
}

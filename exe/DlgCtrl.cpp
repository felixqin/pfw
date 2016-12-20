// DlgCtrl.cpp : ʵ���ļ�
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



// CDlgCtrl ��Ϣ�������


BOOL CDlgCtrl::SetDlg(CDialog * pDlg, UINT nIDTemplate)
{
	ASSERT(pDlg!=NULL);

	//�ǵ�ǰ�������Ѿ���ʾ
	if (pDlg == m_pDlg)
	{
		return TRUE;
	}
	//pDlg ���Ǵ����Ҵ������ɹ�
	if (!IsWindow(pDlg->m_hWnd) &&
			!pDlg->Create(nIDTemplate,this))
	{
		return FALSE;
	}

	//ע��: �뱣֤�Ի���ģ�� Style ��ֵΪ"�Ӽ�"
	ASSERT(IsChild(pDlg));

	//�Ի�����ʾ�л�
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

#include "precomp.h"
#include "global.h"

//===========================================================================
//�й��豸��ȫ�ֱ���
PDRIVER_OBJECT	g_DriverObject = NULL;	//��������
PDEVICE_OBJECT	g_ControlDevice = NULL;	//���ƶ���
PDEVICE_OBJECT	g_TcpDevice = NULL;		//TCP �豸����
PDEVICE_OBJECT	g_UdpDevice = NULL;		//UDP �豸����
PDEVICE_OBJECT	g_IpDevice = NULL;		//IP �豸����

CObjectTbl		g_Objects;				//�ļ������

//===========================================================================

PVOID			g_EventHandlers[MAX_EVENT] = { 0 };	//�������¼�����

//===========================================================================
//	�йع��˹���ȫ�ֱ���

ULONG			g_PfwEnabled = 0;		//�Ƿ�����ʹ�ù��˹���

CFilterRules	g_FilterRules;		//��ȫ����
CAppRules		g_AppRules;			//Ӧ�ó���ȫ����

//CFilter			g_Filter;


/////////////////////////////////////////////////////////////////////////////
//	�й���־��¼��ȫ�ֱ���

CTdiNotify		g_Notify;

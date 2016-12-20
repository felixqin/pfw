// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "d_notify.h"
#include "dll.h"


const TCHAR *g_szServiceName = _T("PFW");


HANDLE		g_hDevice = NULL;
HWND		g_hWnd = NULL;

CAppRules	g_AppRules;
CPidSet		g_Pids;

BOOL		g_bInitialized = FALSE;

CRITICAL_SECTION	g_cs;



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

//���� pfw ����ǽ����
BOOL StartPfwService()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	BOOL succ = FALSE;

	hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if ( hSCManager )
	{
        hService = OpenService( hSCManager, g_szServiceName,
							SERVICE_START | SERVICE_QUERY_STATUS );
		if ( hService )
		{
			SERVICE_STATUS ss;
			if ( QueryServiceStatus( hService, &ss ) )
			{
				succ = TRUE;
				if ( ss.dwCurrentState == SERVICE_STOPPED )
				{
					succ = StartService( hService, NULL, NULL );
				}
			}
			CloseServiceHandle( hService );
		}
		CloseServiceHandle( hSCManager );
	}
	return succ;
}

BOOL PfwInitialize()
{
	BOOL succ = TRUE;

	InitializeCriticalSection( &g_cs );

	//TODO: ��������
	if ( !StartPfwService() )
	{
		MessageBox(NULL,_T("�������� pfw ����ǽ����!"),NULL,MB_ICONERROR);
		succ = FALSE;
		goto done;
	}

	//TODO: ���������豸
	g_hDevice = CreateFile(_T("\\\\.\\pfw_Control"), GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if ( INVALID_HANDLE_VALUE == g_hDevice )
	{
		MessageBox(NULL,_T("Can not open device \\\\.\\pfw_Control"),NULL,MB_ICONERROR);
		succ = FALSE;
		goto done;
	}

	//������а�ȫ����
	DWORD cc;
	succ = DeviceIoControl(g_hDevice, IOCTL_CMD_CLEAR_ALL_RULES, 0, 0, 0,0,&cc,NULL);
	if ( !succ )
	{
		MessageBox( NULL, _T("Can not clear all rules!"), NULL, MB_ICONERROR );
		goto done;
	}

	//TODO: ����֪ͨ, �豸����Ӧ�ò㷢��֪ͨ
	succ = DeviceIoControl(g_hDevice, IOCTL_CMD_NOTIFY_START, 0, 0, 0,0,&cc,NULL);
	if ( !succ )
	{
		MessageBox( NULL, _T("Can not start notification!"), NULL, MB_ICONERROR );
		goto done;
	}

	//TODO: �����ȴ�֪ͨ�߳�
	succ = ( StartWaitNotifyThread() != NULL );

done:
	if ( !succ ) PfwUninitialize();

	DBGPRINTVAR(g_hWnd);
	g_hWnd = NULL;
	g_bInitialized = TRUE;
	return succ;
}
void PfwUninitialize()
{
	//��ֹ�������(���÷���ǽ)
	if (g_bInitialized)
	{
		PfwEnable( FALSE );
	}

	//TODO: ֹ֪ͣͨ
	if ( g_hDevice )
	{
		DWORD cc;
		DeviceIoControl(g_hDevice, IOCTL_CMD_NOTIFY_STOP, 0, 0, 0,0,&cc,NULL);
	}

	//TODO: ��ֹ�ȴ�֪ͨ�߳�
	TerminateWaitNotifyThread();

	//TODO: ɾ�������豸
	if ( g_hDevice )
	{
		CloseHandle( g_hDevice );
		g_hDevice = NULL;
	}

	//������а�ȫ����
	DWORD cc;
	DeviceIoControl(g_hDevice, IOCTL_CMD_CLEAR_ALL_RULES, 0, 0, 0,0,&cc,NULL);

	g_hWnd = NULL;
	g_bInitialized = FALSE;

	DeleteCriticalSection( &g_cs );
}


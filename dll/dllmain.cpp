// dllmain.cpp : 定义 DLL 应用程序的入口点。
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

//启动 pfw 防火墙服务
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

	//TODO: 启动服务
	if ( !StartPfwService() )
	{
		MessageBox(NULL,_T("不能启动 pfw 防火墙服务!"),NULL,MB_ICONERROR);
		succ = FALSE;
		goto done;
	}

	//TODO: 创建控制设备
	g_hDevice = CreateFile(_T("\\\\.\\pfw_Control"), GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if ( INVALID_HANDLE_VALUE == g_hDevice )
	{
		MessageBox(NULL,_T("Can not open device \\\\.\\pfw_Control"),NULL,MB_ICONERROR);
		succ = FALSE;
		goto done;
	}

	//清除所有安全规则
	DWORD cc;
	succ = DeviceIoControl(g_hDevice, IOCTL_CMD_CLEAR_ALL_RULES, 0, 0, 0,0,&cc,NULL);
	if ( !succ )
	{
		MessageBox( NULL, _T("Can not clear all rules!"), NULL, MB_ICONERROR );
		goto done;
	}

	//TODO: 启动通知, 设备可向应用层发送通知
	succ = DeviceIoControl(g_hDevice, IOCTL_CMD_NOTIFY_START, 0, 0, 0,0,&cc,NULL);
	if ( !succ )
	{
		MessageBox( NULL, _T("Can not start notification!"), NULL, MB_ICONERROR );
		goto done;
	}

	//TODO: 启动等待通知线程
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
	//禁止封包过滤(禁用防火墙)
	if (g_bInitialized)
	{
		PfwEnable( FALSE );
	}

	//TODO: 停止通知
	if ( g_hDevice )
	{
		DWORD cc;
		DeviceIoControl(g_hDevice, IOCTL_CMD_NOTIFY_STOP, 0, 0, 0,0,&cc,NULL);
	}

	//TODO: 终止等待通知线程
	TerminateWaitNotifyThread();

	//TODO: 删除控制设备
	if ( g_hDevice )
	{
		CloseHandle( g_hDevice );
		g_hDevice = NULL;
	}

	//清除所有安全规则
	DWORD cc;
	DeviceIoControl(g_hDevice, IOCTL_CMD_CLEAR_ALL_RULES, 0, 0, 0,0,&cc,NULL);

	g_hWnd = NULL;
	g_bInitialized = FALSE;

	DeleteCriticalSection( &g_cs );
}


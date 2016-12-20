#include "stdafx.h"
#include "dll.h"

long WINAPI WaitThread(LPARAM lParam);
long WINAPI DispatchThread(LPARAM lParam);

void OnAppRequest( HANDLE pid );
void OnAppCreate( HANDLE pid );



HANDLE	g_hThread = NULL;
HANDLE	g_hEvent = NULL;


///////////////////////////////////////////////////////////////////////////////////
HANDLE StartWaitNotifyThread()
{
	//TODO: 创建接受设备通知的同步事件
	g_hEvent = OpenEvent( SYNCHRONIZE, FALSE, SHARE_EVENT_NAME_FOR_USER);
	if ( NULL == g_hEvent )
	{
		MessageBox(NULL,_T("Can not create event!"), NULL, MB_ICONERROR);
		return NULL;
	}

	g_hThread = CreateThread( NULL, 0, 
					(LPTHREAD_START_ROUTINE)WaitThread,
					0, 0, 0);
	return g_hThread;
}

void TerminateWaitNotifyThread()
{
	TerminateThread( g_hThread, 0 );
	//TODO: 删除接受设备通知的同步事件
	if ( g_hEvent )
	{
		CloseHandle( g_hEvent );
		g_hEvent = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////
long WINAPI WaitThread( LPARAM )
{
	while (1)
	{
		WaitForSingleObject( g_hEvent, INFINITE );
		//TODO: 创建线程,分发通知
		CreateThread( NULL, 0,
					(LPTHREAD_START_ROUTINE)DispatchThread,
					0, 0, 0);
	}
	return 0;
}

long WINAPI DispatchThread( LPARAM )
{
	DBGPRINT(_T("--->DispatchThread"));
	BYTE buffer[MAX_NOTIFY_INFO_LEN];
	DWORD cc;

	if ( ! DeviceIoControl( g_hDevice, IOCTL_CMD_NOTIFY_GETINFO, 
						0, 0, buffer, sizeof(buffer), &cc, NULL ) )
	{
		return FALSE;
	}
	_ASSERT( cc <= sizeof(buffer) );

	PNOTIFICATION_INFO pInfo = (NOTIFICATION_INFO*)buffer;
	switch ( pInfo->Type )
	{
	case NT_LOG:		//日志通知
		if ( g_hWnd )
		{
			PLOG_INFO pLog = (PLOG_INFO)pInfo->Context;
			SendMessage( g_hWnd, WM_PFW_NOTIFY, (WPARAM)NT_LOG, (LPARAM)pLog );
		}
		break;
	case NT_TDI_REQUEST:	//进程请求传输通知
		{
			HANDLE *ppid = (HANDLE*)pInfo->Context;
			OnAppRequest(*ppid);
		}
		break;
	case NT_PROCESS_CREATE:		//进程创建通知
		{
			HANDLE *ppid = (HANDLE*)pInfo->Context;
			OnAppCreate( *ppid );
		}
		break;
	case NT_PROCESS_TERMINATE:		//进程终止通知
		__try
		{
			EnterCriticalSection( &g_cs );	//进入临界区
			HANDLE *ppid = (HANDLE*)pInfo->Context;
			g_Pids.Delete( *ppid );
		}
		__finally
		{
			LeaveCriticalSection( &g_cs );	//离开临界区
		}
		break;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
//	TODO: 接受底层设备发来的 NT_TDI_REQUEST 通知
void OnAppRequest( HANDLE pid )
{
	BOOL bInserted = FALSE, bFound = FALSE;
	__try
	{
		EnterCriticalSection( &g_cs );	//进入临界区

		bInserted = g_Pids.Insert( pid );
		if ( bInserted )
		{
			LPCTSTR name = g_Pids.GetProcessFileName( pid );
			APP_RULE *pRule = g_AppRules.Find( name );
			if ( pRule )		//有该进程的网络访问规则
			{
				bFound = TRUE;
				pRule->pid = pid;
				DBGPRINTVAR(pRule);
				DBGPRINT(pRule->app_name);
				DBGPRINTVAR(pRule->size);
				//TODO: 控制底层设备添加 rule 规则
				DWORD cc;
				DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_INSERT, 
							pRule, pRule->size, 0, 0, &cc, NULL );
			}
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs );	//离开临界区
	}
	//添加到进程表但未找到应用程序访问规则时通知主控程序
	if ( bInserted && !bFound && g_hWnd )
	{
		//TODO: 向主窗口发送请求网络服务消息
		SendMessage( g_hWnd,WM_PFW_NOTIFY, (WPARAM)NT_TDI_REQUEST, (LPARAM)pid );
	}
}

void OnAppCreate( HANDLE pid )
{
	__try
	{
		EnterCriticalSection( &g_cs );	//进入临界区

		if ( g_Pids.Insert( pid ) )
		{
			LPCTSTR name = g_Pids.GetProcessFileName( pid );
			APP_RULE *pRule = g_AppRules.Find( name );
			DBGPRINT(_T("OnAppCreate"));
			DBGPRINTVAR( pRule );
			if ( pRule )		//有该进程的网络访问规则
			{
				pRule->pid = pid;
				//TODO: 控制底层设备添加 rule 规则
				DWORD cc;
				DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_INSERT, 
							pRule, pRule->size, 0, 0, &cc, NULL );
			}
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs );	//离开临界区
	}
}


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
	//TODO: ���������豸֪ͨ��ͬ���¼�
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
	//TODO: ɾ�������豸֪ͨ��ͬ���¼�
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
		//TODO: �����߳�,�ַ�֪ͨ
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
	case NT_LOG:		//��־֪ͨ
		if ( g_hWnd )
		{
			PLOG_INFO pLog = (PLOG_INFO)pInfo->Context;
			SendMessage( g_hWnd, WM_PFW_NOTIFY, (WPARAM)NT_LOG, (LPARAM)pLog );
		}
		break;
	case NT_TDI_REQUEST:	//����������֪ͨ
		{
			HANDLE *ppid = (HANDLE*)pInfo->Context;
			OnAppRequest(*ppid);
		}
		break;
	case NT_PROCESS_CREATE:		//���̴���֪ͨ
		{
			HANDLE *ppid = (HANDLE*)pInfo->Context;
			OnAppCreate( *ppid );
		}
		break;
	case NT_PROCESS_TERMINATE:		//������ֹ֪ͨ
		__try
		{
			EnterCriticalSection( &g_cs );	//�����ٽ���
			HANDLE *ppid = (HANDLE*)pInfo->Context;
			g_Pids.Delete( *ppid );
		}
		__finally
		{
			LeaveCriticalSection( &g_cs );	//�뿪�ٽ���
		}
		break;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
//	TODO: ���ܵײ��豸������ NT_TDI_REQUEST ֪ͨ
void OnAppRequest( HANDLE pid )
{
	BOOL bInserted = FALSE, bFound = FALSE;
	__try
	{
		EnterCriticalSection( &g_cs );	//�����ٽ���

		bInserted = g_Pids.Insert( pid );
		if ( bInserted )
		{
			LPCTSTR name = g_Pids.GetProcessFileName( pid );
			APP_RULE *pRule = g_AppRules.Find( name );
			if ( pRule )		//�иý��̵�������ʹ���
			{
				bFound = TRUE;
				pRule->pid = pid;
				DBGPRINTVAR(pRule);
				DBGPRINT(pRule->app_name);
				DBGPRINTVAR(pRule->size);
				//TODO: ���Ƶײ��豸��� rule ����
				DWORD cc;
				DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_INSERT, 
							pRule, pRule->size, 0, 0, &cc, NULL );
			}
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs );	//�뿪�ٽ���
	}
	//��ӵ����̱�δ�ҵ�Ӧ�ó�����ʹ���ʱ֪ͨ���س���
	if ( bInserted && !bFound && g_hWnd )
	{
		//TODO: �������ڷ����������������Ϣ
		SendMessage( g_hWnd,WM_PFW_NOTIFY, (WPARAM)NT_TDI_REQUEST, (LPARAM)pid );
	}
}

void OnAppCreate( HANDLE pid )
{
	__try
	{
		EnterCriticalSection( &g_cs );	//�����ٽ���

		if ( g_Pids.Insert( pid ) )
		{
			LPCTSTR name = g_Pids.GetProcessFileName( pid );
			APP_RULE *pRule = g_AppRules.Find( name );
			DBGPRINT(_T("OnAppCreate"));
			DBGPRINTVAR( pRule );
			if ( pRule )		//�иý��̵�������ʹ���
			{
				pRule->pid = pid;
				//TODO: ���Ƶײ��豸��� rule ����
				DWORD cc;
				DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_INSERT, 
							pRule, pRule->size, 0, 0, &cc, NULL );
			}
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs );	//�뿪�ٽ���
	}
}


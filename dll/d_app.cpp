#include "stdafx.h"
#include "dll.h"



/*
����Ӧ�ó���������ýӿں���
*/
DLL_API BOOL PfwAppRuleInsert( APP_RULE *pRule )
{
	_ASSERT( g_bInitialized );
	BOOL succ = FALSE;
	EnterCriticalSection( &g_cs );
	__try
	{
		if ( g_AppRules.Insert( *pRule ) )
		{
			//���ҵ�ǰ���еĽ��̼�������ؽ���
			HANDLE pid = g_Pids.FirstPid();
			while ( pid != 0 )
			{
				LPCTSTR name = g_Pids.GetProcessFileName( pid );
				if ( name != NULL 
					&& (_tcscmp( (LPCTSTR)pRule->app_name, name ) == 0) )
				{
					pRule->pid = pid;
					//TODO: ���Ƶײ��豸��� rule ����
					DWORD cc;
					DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_INSERT, 
								pRule, pRule->size, 0, 0, &cc, NULL );
				}
				pid = g_Pids.NextPid();
			}
			succ = TRUE;
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs);
	}
	return succ;
}
DLL_API BOOL PfwAppRuleDelete( LPCTSTR szAppName )
{
	_ASSERT( g_bInitialized );
	BOOL succ = FALSE;
	EnterCriticalSection( &g_cs );
	__try
	{
		if ( g_AppRules.Delete( szAppName ) )
		{
			HANDLE pid = g_Pids.FirstPid();
			while ( pid != 0 )
			{
				LPCTSTR name = g_Pids.GetProcessFileName( pid );
				if ( name != NULL && _tcscmp( szAppName, name ) == 0 )
				{
					//TODO: ���Ƶײ��豸ɾ�� ���� pid �Ĺ���
					DWORD cc;
					DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_DELETE, 
								&pid, sizeof(pid), 0, 0, &cc, NULL );
				}
				pid = g_Pids.NextPid();
			}
			succ = TRUE;
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs);
	}
	return TRUE;
}
DLL_API BOOL PfwAppRuleModify( APP_RULE *pRule )
{
	_ASSERT( g_bInitialized );
	BOOL succ = FALSE;
	EnterCriticalSection( &g_cs );
	__try
	{
		if ( g_AppRules.Modify( *pRule ) )
		{
			HANDLE pid = g_Pids.FirstPid();
			while ( pid != 0 )
			{
				LPCTSTR name = g_Pids.GetProcessFileName( pid );
				if ( name != NULL 
					&& (_tcscmp( (LPCTSTR)pRule->app_name, name ) == 0) )
				{
					pRule->pid = pid;
					//TODO: ���Ƶײ��豸�޸� rule ����
					DWORD cc;
					DeviceIoControl( g_hDevice, IOCTL_CMD_ARULE_MODIFY, 
								pRule, pRule->size, 0, 0, &cc, NULL );
				}
				pid = g_Pids.NextPid();
			}
			succ = TRUE;
		}
	}
	__finally
	{
		LeaveCriticalSection( &g_cs);
	}
	return TRUE;
}



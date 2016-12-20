#include "stdafx.h"
#include "dll.h"


BOOL PfwFilterRuleInsert( FILTER_RULE *pRule, long nPos )
{
	_ASSERT( g_bInitialized );
	_ASSERT( pRule && nPos>=-1 );

	UINT len = sizeof(FILTER_RULE) + sizeof(long);
	PBYTE buf = new BYTE[len];
	memcpy( buf, &nPos, sizeof(long) );
	memcpy( (PBYTE)buf + sizeof(long), pRule, sizeof(FILTER_RULE) );
	DWORD n;
	BOOL ret = DeviceIoControl(g_hDevice, IOCTL_CMD_FRULE_INSERT, 
						buf, len, 0,0,&n,NULL);

	delete[] buf;
	return ret;
}

DLL_API BOOL PfwFilterRuleDelete( long nPos )
{
	_ASSERT( g_bInitialized );
	_ASSERT( nPos >= 0 );

	DWORD n;
	return DeviceIoControl(g_hDevice, IOCTL_CMD_FRULE_DELETE, 
						&nPos, sizeof(nPos), 0,0,&n,NULL);
}
BOOL PfwFilterRuleModify( FILTER_RULE *pRule, long nPos )
{
	_ASSERT( g_bInitialized );
	_ASSERT( pRule && nPos>=0 );

	UINT len = sizeof(FILTER_RULE) + sizeof(long);
	PBYTE buf = new BYTE[len];
	memcpy( buf, &nPos, sizeof(long) );
	memcpy( (PBYTE)buf + sizeof(long), pRule, sizeof(FILTER_RULE) );
	DWORD n;
	BOOL ret = DeviceIoControl(g_hDevice, IOCTL_CMD_FRULE_MODIFY, 
						buf, len, 0,0,&n,NULL);

	delete[] buf;
	return ret;
}
DLL_API BOOL PfwFilterRuleMoveUp( long nPos )
{
	_ASSERT( g_bInitialized );
	_ASSERT( nPos > 0 );

	long pos = nPos - 1;
	DWORD n;
	return DeviceIoControl(g_hDevice, IOCTL_CMD_FRULE_MOVEDOWN, 
						&pos, sizeof(pos), 0,0,&n,NULL);
}
DLL_API BOOL PfwFilterRuleMoveDown( long nPos )
{
	_ASSERT( g_bInitialized );
	_ASSERT( nPos >= 0 );

	DWORD n;
	return DeviceIoControl(g_hDevice, IOCTL_CMD_FRULE_MOVEDOWN, 
						&nPos, sizeof(nPos), 0,0,&n,NULL);
}



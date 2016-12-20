#include "stdafx.h"
#include "dll.h"




/*
日志及驱动管理接口函数
*/
DLL_API BOOL PfwEnable( BOOL bEnable )
{
	_ASSERT( g_bInitialized );
	DWORD cc;
	return DeviceIoControl(g_hDevice, IOCTL_CMD_PFW_ENABLE, 
						&bEnable, sizeof(bEnable), 0,0,&cc,NULL);

}

DLL_API HWND PfwSetNotifiedWnd( HWND hWnd )
{
	_ASSERT( g_bInitialized );
	HWND hOldWnd = g_hWnd;
	g_hWnd = hWnd;
	return hOldWnd;
}
DLL_API HWND PfwGetNotifiedWnd()
{
	_ASSERT( g_bInitialized );
	return g_hWnd;
}

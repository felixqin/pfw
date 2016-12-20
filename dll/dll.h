#pragma once

#include "d_arule.h"
#include "d_pidset.h"

extern HANDLE		g_hDevice;
extern HANDLE		g_hEvent;

extern HWND			g_hWnd;


extern CAppRules	g_AppRules;
extern CPidSet		g_Pids;


extern BOOL			g_bInitialized;

extern CRITICAL_SECTION	g_cs;

/////////////////////////////////////////////////////

HANDLE StartWaitNotifyThread();
void TerminateWaitNotifyThread();



#ifdef _DEBUG
	#define DBGPRINT(str)	\
		{	\
			OutputDebugString( str );	\
		}
	#define DBGPRINTVAR(var)	\
	{	\
		TCHAR str[100];		\
		_stprintf(str,_T("%s = 0x%08x"),_T(#var),var);	\
		OutputDebugString( str );		\
	}
#else
	#define DBGPRINT(str)
	#define DBGPIRNTVAR(var)
#endif


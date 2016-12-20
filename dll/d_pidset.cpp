#include "stdafx.h"
#include <psapi.h>
#include "d_pidset.h"

///////////////////////////////////////////////////////////////////////////////
UINT GetProcessFileName( HANDLE pid, LPTSTR lpFileName, UINT cchFileName )
{
	HANDLE	hProcess;
	HMODULE	hModule;
	DWORD	cbNeeded;
	UINT	cc = 0;

	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
							PROCESS_VM_READ,
							FALSE, (DWORD)pid );
	if ( hProcess != NULL 
		&& EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
    {
		cc =  GetModuleFileNameEx( hProcess, hModule, lpFileName, cchFileName);
	}

	CloseHandle( hProcess );
	return cc;
}
///////////////////////////////////////////////////////////////////////////////
CPidSet::CPidSet()
{
	ZeroMemory(m_pids,sizeof(m_pids));
	ZeroMemory(m_names,sizeof(m_names));
	m_pos = -1;

	//TODO:添加当前所有运行的进程
	DWORD needed;
	EnumProcesses( (DWORD*)m_pids, sizeof(m_pids), &needed );
}
CPidSet::~CPidSet()
{
	for ( int i=0; i<MAX_RUNED_PROCESS_NUM; i++ )
	{
		delete m_names[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL CPidSet::Insert(HANDLE pid)
{
	_ASSERT(pid);
	int i;
	if ( (i=find(pid)) >= 0 )
	{
		//替换成新进程
		delete[] m_names[i];
		m_names[i] = NULL;
	}
	else
	{
		if ( (i=find(0)) < 0 ) return FALSE;
		m_pids[i] = pid;
	}
	m_pos = i;	//提高 GetProcessFileName 的执行效率
	return TRUE;
}
BOOL CPidSet::Delete(HANDLE pid)
{
	_ASSERT(pid);
	int i = find(pid);
	if (i<0) return FALSE;

	m_pids[i] = 0;
	delete m_names[i];
	m_names[i] = NULL;

	return TRUE;
}
LPCTSTR CPidSet::GetProcessFileName(HANDLE pid)
{
	_ASSERT(pid);
	int i = ( m_pids[m_pos] == pid )? m_pos : find(pid);
	if ( i<0 ) return NULL;

	if ( m_names[i] == NULL )
	{
		m_names[i] = new TCHAR[MAX_PATH];
		UINT cc = ::GetProcessFileName( pid, m_names[i], MAX_PATH );
		if ( cc == 0 )
		{
			delete[] m_names[i];
			m_names[i] = NULL;
			return NULL;
		}
	}
	return m_names[i];
}
///////////////////////////////////////////////////////////////////////////////
HANDLE CPidSet::FirstPid()
{
	for ( m_pos = 0; m_pos < MAX_RUNED_PROCESS_NUM; m_pos++ )
	{
		if ( m_pids[m_pos] != 0 ) break;
	}
	if ( m_pos == MAX_RUNED_PROCESS_NUM) return 0;
	return m_pids[m_pos];
}
HANDLE CPidSet::NextPid()
{
	for ( m_pos++; m_pos < MAX_RUNED_PROCESS_NUM; m_pos++ )
	{
		if ( m_pids[m_pos] != 0 ) break;
	}
	if ( m_pos == MAX_RUNED_PROCESS_NUM)
	{
		m_pos = -1;	//使m_pos无效
		return 0;
	}
	return m_pids[m_pos];
}
///////////////////////////////////////////////////////////////////////////////
inline int CPidSet::find(HANDLE pid)
{
	int i = MAX_RUNED_PROCESS_NUM - 1;
	while ( i>=0 && ( m_pids[i] != pid ) ) i--;
	return i;
}

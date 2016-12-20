#pragma once


#define MAX_RUNED_PROCESS_NUM		512

//系统正在运行的需要网络服务的进程类
//	可能包含一些不访问网络的进程,但不影响运行结果
class CPidSet
{
public:
	BOOL Insert(HANDLE pid);
	BOOL Delete(HANDLE pid);
	LPCTSTR GetProcessFileName(HANDLE pid);

	HANDLE CPidSet::FirstPid();
	HANDLE CPidSet::NextPid();

public:
	CPidSet();
	~CPidSet();

private:
	HANDLE m_pids[MAX_RUNED_PROCESS_NUM];
	LPTSTR m_names[MAX_RUNED_PROCESS_NUM];

	int m_pos;		//游标

private:
	int find(HANDLE pid);	//找到,返回下标.否则返回-1

};


#pragma once


#define MAX_RUNED_PROCESS_NUM		512

//ϵͳ�������е���Ҫ�������Ľ�����
//	���ܰ���һЩ����������Ľ���,����Ӱ�����н��
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

	int m_pos;		//�α�

private:
	int find(HANDLE pid);	//�ҵ�,�����±�.���򷵻�-1

};


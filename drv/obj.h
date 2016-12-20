#pragma once


#define MAX_EVENT	(TDI_EVENT_ERROR_EX + 1)

typedef struct _PFW_EVENT_CONTEXT		//�¼�������
{
	PFILE_OBJECT	fileobj;
	PVOID			old_handler;
	PVOID			old_context;
} PFW_EVENT_CONTEXT, *PPFW_EVENT_CONTEXT;


typedef struct _OBJECT_TABLE_ENTRY
{
	PFILE_OBJECT	fileobj;
	HANDLE			pid;
	ULONG			ip;
	USHORT			port;
	USHORT			reserved;	//˫�ֶ���
	PFW_EVENT_CONTEXT	*handlers;
	_OBJECT_TABLE_ENTRY *next;
} OBJECT_TABLE_ENTRY, *POBJECT_TABLE_ENTRY;



class CObjectTbl
{
public:
	void Initialize();
	void Uninitialize();
	void Clear();

	//���ɾ��fileobj�ļ�����, ͬʱ���� pid, ip, port ��ֵ
	NTSTATUS Append( PFILE_OBJECT fileobj, HANDLE pid, ULONG ip=0, USHORT port=0 );
	NTSTATUS Remove( PFILE_OBJECT fileobj );

	//ȡ��(����)fileobj�ļ������ pid, ip, port ֵ
	NTSTATUS GetInfo( PFILE_OBJECT fileobj, PHANDLE ppid, PULONG pip, PUSHORT pport );
	NTSTATUS SetInfo( PFILE_OBJECT fileobj, PHANDLE ppid, PULONG pip, PUSHORT pport );

	PFW_EVENT_CONTEXT *GetEventHandlers( PFILE_OBJECT fileobj );

private:
	POBJECT_TABLE_ENTRY find( PFILE_OBJECT fileobj, POBJECT_TABLE_ENTRY *pprev=NULL );

private:
	OBJECT_TABLE_ENTRY *m_link;
	KSPIN_LOCK m_guard;

};


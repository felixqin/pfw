#pragma once


#define SHARE_EVENT_NAME_FOR_KERNEL	L"\\BaseNamedObjects\\PFW_EVENT"
#define SHARE_EVENT_NAME_FOR_USER	_T("PFW_EVENT")

//----------------------------------------------------------------------------------

typedef struct _NOTIFICATION_INFO
{
	ULONG	Type;
	USHORT	TotalSize;
	USHORT	ContextSize;
	UCHAR	Context[4];		//ʹ�� 4 , ��Ϊ�˶���
}	NOTIFICATION_INFO, *PNOTIFICATION_INFO;


typedef struct _LOG_INFO
{
	HANDLE		pid;		//����ID
	USHORT		type;		//���ݰ�����,���������������
	USHORT		action;		//����ǽ�Ըð��Ķ���
	ULONG		ip;			//�Է�IP��ַ
	USHORT		lport;		//���ض˿�(local port)
	USHORT		rport;		//�Է��˿�(remote port)
}	LOG_INFO, *PLOG_INFO;

////////////////////////////////////////////////////////////////////////////////////

#define MAX_NOTIFY_INFO_LEN		(sizeof(NOTIFICATION_INFO)+sizeof(LOG_INFO))

////////////////////////////////////////////////////////////////////////////////////
//	NT_xxx		NT:NOTIFICATION TYPE
#define NT_NULL					0
#define NT_LOG					1
#define	NT_TDI_REQUEST			2
#define	NT_PROCESS_CREATE		3
#define NT_PROCESS_TERMINATE	4


////////////////////////////////////////////////////////////////////////////////////
//	MESSAGE MACRO
#define	WM_PFW_NOTIFY					(WM_USER + 0x321)
//#define WM_PFW_LOG					(WM_PFW + 0x01)
//#define WM_PFW_PROCESS_REQUEST		(WM_PFW + 0x10)


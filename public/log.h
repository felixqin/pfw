#pragma once


#define SHARE_EVENT_NAME_FOR_KERNEL	L"\\BaseNamedObjects\\PFW_EVENT"
#define SHARE_EVENT_NAME_FOR_USER	_T("PFW_EVENT")

//----------------------------------------------------------------------------------

typedef struct _NOTIFICATION_INFO
{
	ULONG	Type;
	USHORT	TotalSize;
	USHORT	ContextSize;
	UCHAR	Context[4];		//使用 4 , 是为了对齐
}	NOTIFICATION_INFO, *PNOTIFICATION_INFO;


typedef struct _LOG_INFO
{
	HANDLE		pid;		//进程ID
	USHORT		type;		//数据包类型,意义见规则类型字
	USHORT		action;		//防火墙对该包的动作
	ULONG		ip;			//对方IP地址
	USHORT		lport;		//本地端口(local port)
	USHORT		rport;		//对方端口(remote port)
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


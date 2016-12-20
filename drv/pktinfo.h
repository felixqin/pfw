#pragma once


//数据包信息
typedef struct _PACKET_INFO
{
	HANDLE		pid;		//进程ID
	USHORT		type;		//数据包类型,意义见规则类型字
	USHORT		reserved;	//占位
	ULONG		ip;			//对方IP地址
	USHORT		lport;		//本地端口(local port)
	USHORT		rport;		//对方端口(remote port)
} PACKET_INFO,	*PPACKET_INFO;



#define DBGPRINT_PACKET_INFO(pi)	\
	{	\
		DBGPRINTVAR((pi).pid);	\
		DBGPRINTVAR((pi).type);	\
		DBGPRINTVAR((pi).ip);	\
		DBGPRINTVAR((pi).lport);	\
		DBGPRINTVAR((pi).rport);	\
	}

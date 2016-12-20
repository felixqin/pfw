#pragma once


//���ݰ���Ϣ
typedef struct _PACKET_INFO
{
	HANDLE		pid;		//����ID
	USHORT		type;		//���ݰ�����,���������������
	USHORT		reserved;	//ռλ
	ULONG		ip;			//�Է�IP��ַ
	USHORT		lport;		//���ض˿�(local port)
	USHORT		rport;		//�Է��˿�(remote port)
} PACKET_INFO,	*PPACKET_INFO;



#define DBGPRINT_PACKET_INFO(pi)	\
	{	\
		DBGPRINTVAR((pi).pid);	\
		DBGPRINTVAR((pi).type);	\
		DBGPRINTVAR((pi).ip);	\
		DBGPRINTVAR((pi).lport);	\
		DBGPRINTVAR((pi).rport);	\
	}

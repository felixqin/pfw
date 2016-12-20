#pragma once


//��ȫ����ṹ
typedef struct _FILTER_RULE
{
	LPTSTR	name;			//��������;	����������ģ���в�ʹ��,������
	LPTSTR	description;	//����˵��;	����������ģ���в�ʹ��,������
	USHORT	type;			//����������
	USHORT	action;			//���˶�����
	ULONG	ip;				//�Է�IP��ַ
	ULONG	mask;			//��ַ����
	USHORT	port1;			//��ʼ�˿�;	�����ݰ�����Ϊ in, ��ʾ���ض˿�
	USHORT	port2;			//�����˿�;	�����ݰ�����Ϊ out, ��ʾ�Է��˿�
} FILTER_RULE, *PFILTER_RULE;

/////////////////////////////////////////////////////////////////////////////
/*
	��������������˵��:
	 15                           8   7                           0
	+---------------+---------------+---------------+---------------+
	|   | T | U | I |   |   |DI |DO |   |   |   |   |   |   |   |   |
	+---------------+---------------+---------------+---------------+
	T: TCP   U: UDP   I: IP
	DI: DIRECT IN     DO: DIRECT OUT
*/
//#define	RT_APPRULE			0x8000
#define	RT_TCP				0x4000
#define	RT_UDP				0x2000
#define	RT_IP				0x1000
#define RT_DIRECTOUT		0x0200
#define	RT_DIRECTIN			0x0100
#define RT_DATA				0x0080

//#define	IS_RT_APPRULE(t)	((t)&RT_APPRULE)	//��Ӧ�ó��������
#define	IS_DIRECTIN(x)		((x)&RT_DIRECTIN)	//���뷽����
#define IS_DIRECTOUT(x)		((x)&RT_DIRECTOUT)	//�ǳ�������

#define IS_DATA_TRANS(x)			((x)&RT_DATA)	//�����ݴ���
#define IS_CONNECT_REQUEST(x)		(!(x)&RT_DATA)	//��������������

#define RT_DIRECT_EQUAL(t1,t2)			(((t1)&0x0F00) == ((t2)&0x0F00))
#define RT_PROTO_EQUAL(t1,t2)			(((t1)&0xF000) == ((t2)&0xF000))
//#define RT_PROTO_DIRECT_EQUAL(t1,t2)	(((t1)&0xFF00) == ((t2)&0xFF00))

/////////////////////////////////////////////////////////////////////////////
/*
	���˶���������˵��:
	 15                           8   7                           0
	+---------------+---------------+---------------+---------------+
	| C | D |   |   |   |   |   |   | R | F | B |   |   |   |   |   |
	+---------------+---------------+---------------+---------------+
	C(Continue): Ϊ1��ʾ���������һ������
	D(Deny): 0��ʾ�������ݰ�ͨ��,1��ʾ��ֹͨ��
	R(Record): 1��ʾ��¼��־
	F(Flash): 1��ʾ��˸ͼ��
	B(Beep): 1��ʾ��������
*/

#define ACTION_CONTINUE		0x8000
#define ACTION_DENY			0x4000
#define ACTION_RECORD		0x0080
#define ACTION_FLASH		0x0040
#define ACTION_BEEP			0x0020

#define	ACTION_PASS			0	//����;����¼,����˸,������

#define IS_ACTION_CONTINUE(x)	((x)&ACTION_CONTINUE)
#define IS_ACTION_PASS(x)		(!((x)&ACTION_DENY))

#define ACTION_LOG_NBEEP		ACTION_RECORD | ACTION_FLASH
#define ACTION_PASS_LOG_NBEEP	ACTION_PASS | ACTION_LOG_NBEEP
#define ACTION_DENY_LOG_NBEEP	ACTION_DENY | ACTION_LOG_NBEEP

#define NEED_LOG(x)		(!!((x)&(ACTION_RECORD|ACTION_FLASH|ACTION_BEEP)))

//ȡ�õ�8λ
#define GET_ACTION_LOG_INFO(x)	((x)&0x00FF)

////////////////////////////////////////////////////////////////////////////
//Ӧ�ó���ȫ����ṹ
//	���port_num����0,��ʾ�ɷ��ʶ˿ڴ�ports[0]��ports[1],��ports[0]
//	��ports[1]��Ϊ0,��ʾ�޶˿ڿɷ���
//	���port_num����0,ports[port_num]�����з��ʵĶ˿��б�
typedef struct _APP_RULE
{
	ULONG	size;			//�ṹ��С
	LPTSTR	app_name;		//Ӧ�ó����ļ�����	��Ӧ�ò�ʹ��
	HANDLE	pid;			//����ID��(�ں��Ĳ�ʹ��,��DLL��exe�в�ʹ��)
	USHORT	allow;			//����Ĳ���(Э�鼰����,�������)
	USHORT	action;			//���˶�����; ����������ʱ�Ķ���
	USHORT	pt_tcp;			//pt(port list type): �˿��б�����
	USHORT	pt_udp;			//PT_LIST		PT_RANGE
	USHORT	pn_tcp;			//pn(port number): �˿������С
	USHORT	pn_udp;			//
	USHORT	ports[2];		//�ɷ��ʶ˿��б�(���4��Ϊ�˶���)
} APP_RULE, *PAPP_RULE;

/*
	pt_xxx ������
*/
#define PT_NULL			0		//�޶˿ڿɷ���
#define PT_LIST			1		//�˿�������һϵ�ж˿ں�
#define PT_RANGE		2		//�˿�����ָ����һ����Χ

/*
	allow ������
*/
#define	ALLOW_TCP_IN		0x0001	//������� TCP ����
#define	ALLOW_TCP_OUT		0x0002	//������ TCP ����
#define	ALLOW_UDP_IN		0x0010	//������� UDP ����
#define	ALLOW_UDP_OUT		0x0020	//������ UDP ����

#define ALLOW_TCP_ALL		ALLOW_TCP_IN | ALLOW_TCP_OUT
#define ALLOW_UDP_ALL		ALLOW_UDP_IN | ALLOW_UDP_OUT
#define ALLOW_ALL			ALLOW_TCP_ALL | ALLOW_UDP_ALL


#define IS_ALLOWED(allow,type)	\
	(			\
					(  ((type)&RT_TCP) && IS_DIRECTIN(type)  && ((allow)&ALLOW_TCP_IN )  )	\
				||	(  ((type)&RT_TCP) && IS_DIRECTOUT(type) && ((allow)&ALLOW_TCP_OUT)  )	\
				||	(  ((type)&RT_UDP) && IS_DIRECTIN(type)  && ((allow)&ALLOW_UDP_IN )  )	\
				||	(  ((type)&RT_UDP) && IS_DIRECTOUT(type) && ((allow)&ALLOW_UDP_OUT)  )	\
	)




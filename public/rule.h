#pragma once


//安全规则结构
typedef struct _FILTER_RULE
{
	LPTSTR	name;			//规则名称;	在驱动程序模块中不使用,无意义
	LPTSTR	description;	//规则说明;	在驱动程序模块中不使用,无意义
	USHORT	type;			//规则类型字
	USHORT	action;			//过滤动作字
	ULONG	ip;				//对方IP地址
	ULONG	mask;			//地址掩码
	USHORT	port1;			//起始端口;	当数据包方向为 in, 表示本地端口
	USHORT	port2;			//结束端口;	当数据包方向为 out, 表示对方端口
} FILTER_RULE, *PFILTER_RULE;

/////////////////////////////////////////////////////////////////////////////
/*
	规则类型字意义说明:
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

//#define	IS_RT_APPRULE(t)	((t)&RT_APPRULE)	//是应用程序规则吗
#define	IS_DIRECTIN(x)		((x)&RT_DIRECTIN)	//是入方向吗
#define IS_DIRECTOUT(x)		((x)&RT_DIRECTOUT)	//是出方向吗

#define IS_DATA_TRANS(x)			((x)&RT_DATA)	//是数据传送
#define IS_CONNECT_REQUEST(x)		(!(x)&RT_DATA)	//是其它网络请求

#define RT_DIRECT_EQUAL(t1,t2)			(((t1)&0x0F00) == ((t2)&0x0F00))
#define RT_PROTO_EQUAL(t1,t2)			(((t1)&0xF000) == ((t2)&0xF000))
//#define RT_PROTO_DIRECT_EQUAL(t1,t2)	(((t1)&0xFF00) == ((t2)&0xFF00))

/////////////////////////////////////////////////////////////////////////////
/*
	过滤动作字意义说明:
	 15                           8   7                           0
	+---------------+---------------+---------------+---------------+
	| C | D |   |   |   |   |   |   | R | F | B |   |   |   |   |   |
	+---------------+---------------+---------------+---------------+
	C(Continue): 为1表示继续检查下一条规则
	D(Deny): 0表示允许数据包通行,1表示禁止通行
	R(Record): 1表示记录日志
	F(Flash): 1表示闪烁图标
	B(Beep): 1表示发声报警
*/

#define ACTION_CONTINUE		0x8000
#define ACTION_DENY			0x4000
#define ACTION_RECORD		0x0080
#define ACTION_FLASH		0x0040
#define ACTION_BEEP			0x0020

#define	ACTION_PASS			0	//放行;不记录,不闪烁,不发声

#define IS_ACTION_CONTINUE(x)	((x)&ACTION_CONTINUE)
#define IS_ACTION_PASS(x)		(!((x)&ACTION_DENY))

#define ACTION_LOG_NBEEP		ACTION_RECORD | ACTION_FLASH
#define ACTION_PASS_LOG_NBEEP	ACTION_PASS | ACTION_LOG_NBEEP
#define ACTION_DENY_LOG_NBEEP	ACTION_DENY | ACTION_LOG_NBEEP

#define NEED_LOG(x)		(!!((x)&(ACTION_RECORD|ACTION_FLASH|ACTION_BEEP)))

//取得低8位
#define GET_ACTION_LOG_INFO(x)	((x)&0x00FF)

////////////////////////////////////////////////////////////////////////////
//应用程序安全规则结构
//	如果port_num等于0,表示可访问端口从ports[0]到ports[1],当ports[0]
//	和ports[1]都为0,表示无端口可访问
//	如果port_num大于0,ports[port_num]是所有访问的端口列表
typedef struct _APP_RULE
{
	ULONG	size;			//结构大小
	LPTSTR	app_name;		//应用程序文件名；	在应用层使用
	HANDLE	pid;			//进程ID；(在核心层使用,在DLL和exe中不使用)
	USHORT	allow;			//允许的操作(协议及方向,意义见下)
	USHORT	action;			//过滤动作字; 不符合条件时的动作
	USHORT	pt_tcp;			//pt(port list type): 端口列表类型
	USHORT	pt_udp;			//PT_LIST		PT_RANGE
	USHORT	pn_tcp;			//pn(port number): 端口数组大小
	USHORT	pn_udp;			//
	USHORT	ports[2];		//可访问端口列表(设成4是为了对齐)
} APP_RULE, *PAPP_RULE;

/*
	pt_xxx 的意义
*/
#define PT_NULL			0		//无端口可访问
#define PT_LIST			1		//端口数组是一系列端口号
#define PT_RANGE		2		//端口数组指定了一个范围

/*
	allow 的意义
*/
#define	ALLOW_TCP_IN		0x0001	//允许接收 TCP 数据
#define	ALLOW_TCP_OUT		0x0002	//允许发送 TCP 数据
#define	ALLOW_UDP_IN		0x0010	//允许接收 UDP 数据
#define	ALLOW_UDP_OUT		0x0020	//允许发送 UDP 数据

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




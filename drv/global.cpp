#include "precomp.h"
#include "global.h"

//===========================================================================
//有关设备的全局变量
PDRIVER_OBJECT	g_DriverObject = NULL;	//驱动对象
PDEVICE_OBJECT	g_ControlDevice = NULL;	//控制对象
PDEVICE_OBJECT	g_TcpDevice = NULL;		//TCP 设备对象
PDEVICE_OBJECT	g_UdpDevice = NULL;		//UDP 设备对象
PDEVICE_OBJECT	g_IpDevice = NULL;		//IP 设备对象

CObjectTbl		g_Objects;				//文件对象表

//===========================================================================

PVOID			g_EventHandlers[MAX_EVENT] = { 0 };	//驱动的事件函数

//===========================================================================
//	有关过滤规则全局变量

ULONG			g_PfwEnabled = 0;		//是否允许使用过滤规则

CFilterRules	g_FilterRules;		//安全规则
CAppRules		g_AppRules;			//应用程序安全规则

//CFilter			g_Filter;


/////////////////////////////////////////////////////////////////////////////
//	有关日志记录的全局变量

CTdiNotify		g_Notify;

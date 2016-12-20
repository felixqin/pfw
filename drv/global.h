#pragma once

#include "obj.h"
#include "frule.h"
#include "arule.h"
#include "notify.h"


//===================================================================================
extern PDRIVER_OBJECT	g_DriverObject;
extern PDEVICE_OBJECT	g_ControlDevice;
extern PDEVICE_OBJECT	g_TcpDevice;
extern PDEVICE_OBJECT	g_UdpDevice;
extern PDEVICE_OBJECT	g_IpDevice;

extern CObjectTbl		g_Objects;

//===================================================================================

extern PVOID g_EventHandlers[MAX_EVENT];


//===================================================================================
//	过滤规则全局变量
extern ULONG			g_PfwEnabled;
extern CFilterRules		g_FilterRules;
extern CAppRules		g_AppRules;

/////////////////////////////////////////////////////////////////////////////////////
//	
extern CTdiNotify		g_Notify;




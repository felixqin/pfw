#include "precomp.h"
#include "dispatch.h"
#include "device.h"
#include "global.h"

VOID ProcessNotify( IN HANDLE  ParentId, 
					IN HANDLE  ProcessId,
					IN BOOLEAN  Create );

VOID Unload(IN PDRIVER_OBJECT DriverObject);

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
					 IN PUNICODE_STRING RegistryPath)
{
	DBGPRINT("==>DriverEntry Loading...\n");

	NTSTATUS status;
	//////////////////////////////////
	//全局变量初始化
	g_DriverObject = DriverObject;

	g_Objects.Initialize();

	g_PfwEnabled = 0;
	g_FilterRules.Initialize();
	g_AppRules.Initialize();

	g_Notify.Initialize();

	///////////////////////////////////
	//设置事件处理函数
	//g_EventHandlers[TDI_EVENT_XXX] = TdiEventXxx;
	g_EventHandlers[TDI_EVENT_CONNECT] = TdiEventConnect;
	g_EventHandlers[TDI_EVENT_RECEIVE] = TdiEventReceive;
	g_EventHandlers[TDI_EVENT_RECEIVE_DATAGRAM] = TdiEventReceiveDatagram;

	///////////////////////////////////
	//驱动对象设置
	DriverObject->DriverUnload = Unload;
	for (int i=0; i<=IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = TdiDispatch;

	///////////////////////////////////////////////////////////////
	//设备创建

	//创建PFW防火墙控制设备
	UNICODE_STRING name, linkname;

	RtlInitUnicodeString(&name, L"\\Device\\pfw_Control");
	status = IoCreateDevice( DriverObject,
							0,	&name,
							0,	0,	TRUE,		// exclusive! only one control app!
							&g_ControlDevice);
	if ( !NT_SUCCESS(status) ) {
		DBGPRINT("DriverEntry: IoCreateDevice(control) failure");
		goto done;
	}

	RtlInitUnicodeString(&linkname, L"\\??\\pfw_Control");
	status = IoCreateSymbolicLink(&linkname, &name);
	if ( !NT_SUCCESS(status) ) {
		DBGPRINT("DriverEntry: IoCreateSymbolicLink failure");
		goto done;
	}

	//////////////////////////////////////////////////////////////
	//创建并挂接 TCP, UDP, IP 设备

	status = create_attach_device( DriverObject,
								L"\\Device\\Tcp",
								L"\\Device\\pfw_Tcp",
								&g_TcpDevice );
	if ( !NT_SUCCESS(status) )
	{
		DBGPRINT("Can not create and attach device pfw_Tcp");
		goto done;
	}
	status = create_attach_device( DriverObject,
								L"\\Device\\Udp",
								L"\\Device\\pfw_Udp",
								&g_UdpDevice );
	if ( !NT_SUCCESS(status) )
	{
		DBGPRINT("Can not create and attach device pfw_Udp");
		goto done;
	}
	status = create_attach_device( DriverObject,
								L"\\Device\\RawIp",
								L"\\Device\\pfw_Ip",
								&g_IpDevice );
	if ( !NT_SUCCESS(status) )
	{
		DBGPRINT("Can not create and attach device pfw_Ip");
		goto done;
	}

	//////////////////////////////////////////////////////////////////////
	//进程创建的通知程序
	status = PsSetCreateProcessNotifyRoutine( ProcessNotify, FALSE );
	if ( !NT_SUCCESS(status) )
	{
		DBGPRINT("Can not set create process notify routine!");
		goto done;
	}

	status = STATUS_SUCCESS;
done:
	if ( !NT_SUCCESS(status) )	Unload( DriverObject );
	return status;
}

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	DBGPRINT("==>Driver Unloading...");
	
	g_Objects.Uninitialize();
	g_FilterRules.Uninitialize();
	g_AppRules.Uninitialize();

	g_Notify.Uninitialize();

	PsSetCreateProcessNotifyRoutine( ProcessNotify, TRUE );

	delete_detach_device( DriverObject, g_TcpDevice );
	delete_detach_device( DriverObject, g_UdpDevice );
	delete_detach_device( DriverObject, g_IpDevice );

	if ( g_ControlDevice != NULL )
	{
		UNICODE_STRING linkname;
		RtlInitUnicodeString(&linkname, L"\\??\\pfw_Control");
		IoDeleteSymbolicLink(&linkname);
		IoDeleteDevice(g_ControlDevice);
	}
}

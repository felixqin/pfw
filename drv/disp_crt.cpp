#include "precomp.h"
#include "dispatch.h"
#include "device.h"
#include "global.h"
#include "sock.h"

//////////////////////////////////////////////////////////////////////////////////////


NTSTATUS TdiCreateNormalComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context);

NTSTATUS TdiCreateSkipComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context);

NTSTATUS TdiCreateComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context);

NTSTATUS QueryAddressComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context);

//////////////////////////////////////////////////////////////////////////////////////
//	进程创建时的通知程序
VOID ProcessNotify( IN HANDLE  ParentId, 
			IN HANDLE  ProcessId,IN BOOLEAN  Create )
{
	if ( Create )
	{
		DBGPRINTVART("Proccess is created! ",ProcessId);
		g_Notify.Notify( NT_PROCESS_CREATE, &ProcessId, sizeof(ProcessId) );
	}
	else
	{
		DBGPRINTVART("Proccess is terminated! ",ProcessId);
		g_AppRules.Delete( ProcessId );
		g_Notify.Notify( NT_PROCESS_TERMINATE, &ProcessId, sizeof(ProcessId) );
	}
}

//////////////////////////////////////////////////////////////////////////////////////

NTSTATUS TdiDispatchCreate( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	DBGPRINT("Dispatch(IRP_MJ_CREATE)...");
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	FILE_FULL_EA_INFORMATION *ffei = (FILE_FULL_EA_INFORMATION *)Irp->AssociatedIrp.SystemBuffer;

	if ( NULL != ffei )
	{
		DBGPRINTVAR( irps->FileObject );
		DBGPRINTVAR(ffei->EaName);
		DBGPRINT((char*)(ffei->EaName));
		// 如果创建的是地址(TransportAddress)对象
		if ( ffei->EaNameLength == TDI_TRANSPORT_ADDRESS_LENGTH 
			&& RtlEqualMemory(ffei->EaName, TdiTransportAddress, TDI_TRANSPORT_ADDRESS_LENGTH) )
		{
			HANDLE pid = PsGetCurrentProcessId();
			status = g_Objects.Append( irps->FileObject, pid );
			DBGPRINT("g_Objects.Append() status:");
			DBGPRINTVAR(status);

			PIRP query_irp = TdiBuildInternalDeviceControlIrp(TDI_QUERY_INFORMATION,
								irps->DeviceObject, irps->FileObject, NULL, NULL);
			if (NULL == query_irp)
			{
				DBGPRINT(("TdiDispatchCreate: TdiBuildInternalDeviceControlIrp failure!"));
				status = Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
				IoCompleteRequest (Irp, IO_NO_INCREMENT);
				return status;
			}

			if ( !g_AppRules.Exist( pid ) )
			{
				//通知应用层程序,有进程请求建立应用程序访问规则
				g_Notify.Notify( NT_TDI_REQUEST, &pid, sizeof(pid) );
			}

			return  DispatchToLowerDevice( DeviceObject, Irp, 
							TdiCreateNormalComplete, TdiCreateSkipComplete, 
							query_irp);
		}
		// 如果创建的是联结(ConnectionContext)对象
		else if ( ffei->EaNameLength == TDI_CONNECTION_CONTEXT_LENGTH 
			&& RtlEqualMemory(ffei->EaName, TdiConnectionContext, TDI_CONNECTION_CONTEXT_LENGTH) )
		{
			status = g_Objects.Append( irps->FileObject, PsGetCurrentProcessId() );
			DBGPRINT("g_Objects.Append() status:");
			DBGPRINTVAR(status);
			return DispatchToLowerDevice( DeviceObject, Irp );
		}
	}

	return DispatchToLowerDevice( DeviceObject, Irp );
}

NTSTATUS TdiCreateNormalComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context)
{
	DBGPRINT("==>TdiCreateNormalComplete");

	TdiCreateComplete( DeviceObject, Irp, Context );
	DBGPRINT("<--TdiCreateComplete");
	return TdiNormalComplete( DeviceObject, Irp, /*Context*/NULL );
}

NTSTATUS TdiCreateSkipComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context)
{
	DBGPRINT("==>TdiCreateSkipComplete");

	PTDI_SKIP_CONTEXT ctx = (PTDI_SKIP_CONTEXT)Context;
	TdiCreateComplete( DeviceObject, Irp, ctx->UserContext );
	DBGPRINT("<--TdiCreateComplete");
	return TdiSkipComplete( DeviceObject, Irp, Context );
}

////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS TdiCreateComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context)
{
	DBGPRINT("==>TdiCreateComplete");
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irps = IoGetCurrentIrpStackLocation(Irp);
	PIRP query_irp = (PIRP)Context;

	TDI_ADDRESS_INFO *tai 
		= (TDI_ADDRESS_INFO *)ExAllocatePool( NonPagedPool, TDI_ADDRESS_INFO_MAX );
	if ( NULL == tai )
	{
		DBGPRINT("TdiCreateComplete: Can not allocate resource to tai!");
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto done;
	}
	PMDL mdl 
		= IoAllocateMdl(tai, TDI_ADDRESS_INFO_MAX, FALSE, FALSE, NULL);
	if ( NULL == mdl )
	{
		DBGPRINT("TdiCreateComplete: IoAllocateMdl failure");
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto done;
	}
	MmBuildMdlForNonPagedPool(mdl);

	TdiBuildQueryInformation(query_irp, DeviceObject, irps->FileObject,
							QueryAddressComplete, &irps->FileObject,
							TDI_QUERY_ADDRESS_INFO, mdl);
	DBGPRINTVAR(irps->FileObject);
	status = IoCallDriver(DeviceObject, query_irp);
	if ( !NT_SUCCESS(status) ) 
	{
		DBGPRINT("TdiCreateComplete: IoCallDriver failure!");
		DBGPRINTVAR(status);
	}
	query_irp = NULL;
	tai = NULL;

done:
	if (query_irp) IoFreeIrp(query_irp);
	if (tai) ExFreePool(tai);

	Irp->IoStatus.Status = status;

	return status;
}
NTSTATUS QueryAddressComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context)
{
	DBGPRINT("==>QueryAddressComplete()");
	PFILE_OBJECT fileobj = *((PFILE_OBJECT*)Context);
	//TDI_ADDRESS_INFO *tai_xxx = (TDI_ADDRESS_INFO*)Context;
	TDI_ADDRESS_INFO *tai
		= (TDI_ADDRESS_INFO *)MmGetSystemAddressForMdlSafe( Irp->MdlAddress, NormalPagePriority );
	sockaddr_in *addr = (sockaddr_in*)(&tai->Address.Address->AddressType);

	DBGPRINTVAR(fileobj);
	//PIO_STACK_LOCATION irps = IoGetCurrentIrpStackLocation(Irp);
	//DBGPRINTVAR(irps->FileObject);
	if (addr->sin_family == AF_INET )
	{
		ULONG ip = ntohl(addr->sin_addr.s_addr);
		USHORT port = ntohs(addr->sin_port);
		DBGPRINTVAR(ip);
		DBGPRINTVAR(port);
		NTSTATUS status = g_Objects.SetInfo( fileobj, NULL, &ip, &port );
		DBGPRINTVAR(status);
	}

	// cleanup MDL
	if (Irp->MdlAddress)
	{
		IoFreeMdl(Irp->MdlAddress);
		Irp->MdlAddress = NULL;
	}
	DBGPRINT("<--QueryAddressComplete()");
	return STATUS_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//
NTSTATUS TdiDispatchCleanup( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	DBGPRINT("Dispatch(IRP_MJ_CLEANUP)...");
	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	DBGPRINTVAR(irps->FileObject);

	HANDLE pid;
	g_Objects.GetInfo( irps->FileObject, &pid, NULL, NULL);

	//删除对象表中的相关条目
	NTSTATUS status = g_Objects.Remove( irps->FileObject );
	DBGPRINTVAR(status);

	DBGPRINTVAR(pid);
	//if ( pid )
	//{
	//	//删除与该进程有关的应用程序规则
	//	g_AppRules.Delete( pid );
	//	//通知应用层程序
	//	g_Notify.Notify( NT_TDI_CLEANUP, &pid, sizeof(pid) );
	//}

	return DispatchToLowerDevice( DeviceObject, Irp );
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//
NTSTATUS TdiDispatchAssociate( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_ASSOCIATE_ADDRESS])...");

	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	HANDLE addr_handle = ((TDI_REQUEST_KERNEL_ASSOCIATE *)(&irps->Parameters))->AddressHandle;
	PFILE_OBJECT addrobj;
	NTSTATUS status;

	status = ObReferenceObjectByHandle(addr_handle, GENERIC_READ, NULL, KernelMode, (PVOID*)&addrobj, NULL);
	if ( !NT_SUCCESS(status) )
	{
		DBGPRINTVAR(status);
		DBGPRINT("TdiDispatchAssociate: ObReferenceObjectByHandle failure!");
	}
	else
	{
		HANDLE pid = 0;
		ULONG ip = 0;
		USHORT port = 0;
		status = g_Objects.GetInfo( addrobj, &pid, &ip, &port );
		DBGPRINTVART("g_Objects.GetInfo",status);
		status = g_Objects.SetInfo( irps->FileObject, &pid, &ip, &port );
		DBGPRINTVART("g_Objects.SetInfo",status);
		DBGPRINTVAR3(pid,ip,port);
	}
	return DispatchToLowerDevice( DeviceObject, Irp );
}

NTSTATUS TdiDispatchDisassociate( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_DISASSOCIATE_ADDRESS])...");

	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	HANDLE pid = 0;
	ULONG ip = 0;
	USHORT port = 0;
	NTSTATUS status = g_Objects.SetInfo( irps->FileObject, &pid, &ip, &port );
	DBGPRINTVAR(status);

	return DispatchToLowerDevice( DeviceObject, Irp );
}


#include "precomp.h"
#include "dispatch.h"
#include "sock.h"
#include "filter.h"
#include "global.h"

NTSTATUS TdiDispatchConnect( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	NTSTATUS status;
	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	PTDI_REQUEST_KERNEL_CONNECT param = (PTDI_REQUEST_KERNEL_CONNECT)(&irps->Parameters);
	TA_ADDRESS *remote_addr = ((TRANSPORT_ADDRESS *)(param->RequestConnectionInformation->RemoteAddress))->Address;
	sockaddr_in *remote_saddr = (sockaddr_in*)(&remote_addr->AddressType);

	//协议类型
	USHORT proto;
	if ( DeviceObject == g_TcpDevice )
		proto = RT_TCP;
	else if ( DeviceObject == g_UdpDevice )
		proto = RT_UDP;
	else
		proto = RT_IP;

	HANDLE pid = PsGetCurrentProcessId();
	ULONG ip = 0;
	USHORT port = 0;
	status = g_Objects.GetInfo( irps->FileObject, &pid, &ip, &port );
	DBGPRINTVAR(status);

	//数据包信息
	PACKET_INFO pi;
	pi.pid = pid;
	pi.type = proto | RT_DIRECTOUT;
	pi.ip = ntohl( remote_saddr->sin_addr.s_addr );
	pi.lport = port;
	pi.rport = ntohs(remote_saddr->sin_port);
	DBGPRINT_PACKET_INFO(pi);

	// TODO: 加入过滤代码
	USHORT action = Filter(&pi);
	DBGPRINTVAR(action);
	// TODO: 加入记录日志代码
	status = Log( action, &pi );
	DBGPRINTVART( "Log(action,&pi)", status );

	if ( IS_ACTION_PASS(action) )
		return DispatchToLowerDevice( DeviceObject, Irp );

	status = Irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
	IoCompleteRequest (Irp, IO_NO_INCREMENT);

	return status;
}
NTSTATUS TdiDispatchSend( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	NTSTATUS status;
	USHORT action = ACTION_PASS;

	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	////PTDI_REQUEST_KERNEL_CONNECT param = (PTDI_REQUEST_KERNEL_CONNECT)(&irps->Parameters);
	////TA_ADDRESS *remote_addr = ((TRANSPORT_ADDRESS *)(param->RequestConnectionInformation->RemoteAddress))->Address;
	////sockaddr_in *remote_saddr = (sockaddr_in*)(&remote_addr->AddressType);
	//PFILE_OBJECT addrobj = irps->FileObject;
	//PTDI_ADDRESS_IP remote_addr = (PTDI_ADDRESS_IP)addrobj->FsContext;

	//DBGPRINT("-------------------------------------------------------------");
	//PIRP query_irp = NULL;
	//PMDL mdl = NULL;

	//query_irp = TdiBuildInternalDeviceControlIrp(TDI_QUERY_INFORMATION,
	//				irps->DeviceObject, irps->FileObject, NULL, NULL);
	//if (query_irp == NULL)
	//{
	//	action = ACTION_PASS;
	//	goto done;
	//}
	//TDI_ADDRESS_INFO *tai = (TDI_ADDRESS_INFO *)ExAllocatePool(NonPagedPool,TDI_ADDRESS_INFO_MAX);
	//if (tai == NULL)
	//{
	//	action = ACTION_PASS;
	//	goto done;
	//}
	//mdl = IoAllocateMdl(tai, TDI_ADDRESS_INFO_MAX, FALSE, FALSE, NULL);
	//if (mdl == NULL)
	//{
	//	ExFreePool(tai);
	//	action = ACTION_PASS;
	//	goto done;
	//}
	//MmBuildMdlForNonPagedPool(mdl);

	//TdiBuildQueryInformation(query_irp, DeviceObject, irps->FileObject,
	//						NULL, NULL,	TDI_QUERY_ADDRESS_INFO, mdl);

	////status = IoCallDriver(DeviceObject, query_irp);
	//if ( NT_SUCCESS(status) )
	//{
	//	//TDI_ADDRESS_INFO *tai2
	//	//	= (TDI_ADDRESS_INFO *)MmGetSystemAddressForMdlSafe( mdl, NormalPagePriority );

	//	DBGPRINTVAR(tai);
	//	//DBGPRINTVAR(tai2);
	//	sockaddr_in *remote_saddr = (sockaddr_in*)(tai->Address.Address->AddressType);

	//	HANDLE pid = PsGetCurrentProcessId();
	//	ULONG ip = 0;
	//	USHORT port = 0;
	//	status = g_Objects.GetInfo( irps->FileObject, &pid, &ip, &port );
	//	DBGPRINTVAR(status);

	//	PACKET_INFO pi;
	//	pi.pid = pid;
	//	pi.type = RT_TCP | RT_DIRECTOUT;
	//	pi.ip = ntohl( remote_saddr->sin_addr.s_addr );
	//	pi.lport = port;
	//	pi.rport = ntohs(remote_saddr->sin_port);
	//	DBGPRINT_PACKET_INFO(pi);
	//}

	//DBGPRINTVAR(mdl);
	//if (mdl) IoFreeMdl(mdl);
	//ExFreePool(tai);
	//DBGPRINT("-------------------------------------------------------------");

	//// TODO: 加入过滤代码
	//USHORT action = Filter(&pi);
	//DBGPRINTVAR(action);
	//// TODO: 加入记录日志代码
	//status = Log( action, &pi );
	//DBGPRINTVART( "Log(action,&pi)", status );

done:
	if ( IS_ACTION_PASS(action) )
		return DispatchToLowerDevice( DeviceObject, Irp );

	status = Irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
	IoCompleteRequest (Irp, IO_NO_INCREMENT);

	return status;
}
NTSTATUS TdiDispatchSendDatagram( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	NTSTATUS status;
	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	PTDI_REQUEST_KERNEL_SENDDG param = (TDI_REQUEST_KERNEL_SENDDG*)(&irps->Parameters);
	TA_ADDRESS *remote_addr = ((TRANSPORT_ADDRESS *)(param->SendDatagramInformation->RemoteAddress))->Address;
	sockaddr_in *remote_saddr = (sockaddr_in*)(&remote_addr->AddressType);

	//协议类型
	USHORT proto;
	if ( DeviceObject == g_TcpDevice )
		proto = RT_TCP;
	else if ( DeviceObject == g_UdpDevice )
		proto = RT_UDP;
	else
		proto = RT_IP;

	HANDLE pid = PsGetCurrentProcessId();
	ULONG ip = 0;
	USHORT port = 0;
	status = g_Objects.GetInfo( irps->FileObject, &pid, &ip, &port );
	DBGPRINTVAR(status);

	//数据包信息
	PACKET_INFO pi;
	pi.pid = pid;
	pi.type = proto | RT_DIRECTOUT;
	pi.ip = ntohl( remote_saddr->sin_addr.s_addr );
	pi.lport = port;
	pi.rport = ntohs(remote_saddr->sin_port);
	DBGPRINT_PACKET_INFO(pi);

	// TODO: 加入过滤代码
	USHORT action = Filter(&pi);
	DBGPRINTVAR(action);
	// TODO: 加入记录日志代码
	status = Log( action, &pi );
	DBGPRINTVART( "Log(action,&pi)", status );

	if ( IS_ACTION_PASS(action) )
		return DispatchToLowerDevice( DeviceObject, Irp );

	status = Irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
	IoCompleteRequest (Irp, IO_NO_INCREMENT);

	return status;
}

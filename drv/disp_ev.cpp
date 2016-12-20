#include "precomp.h"
#include "dispatch.h"
#include "sock.h"
#include "filter.h"
#include "global.h"


////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS TdiDispatchSetEvent( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp )
{
	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	PTDI_REQUEST_KERNEL_SET_EVENT r = (PTDI_REQUEST_KERNEL_SET_EVENT)&irps->Parameters;

	PFW_EVENT_CONTEXT *handlers = g_Objects.GetEventHandlers( irps->FileObject );
	PVOID pfw_handler = g_EventHandlers[r->EventType];
	DBGPRINTVAR(irps->FileObject);
	DBGPRINTVAR3( pfw_handler, handlers, r->EventHandler );

	if ( handlers != NULL && pfw_handler != NULL )
	{
		if ( r->EventHandler != NULL )
		{
			handlers[r->EventType].fileobj = irps->FileObject;
			handlers[r->EventType].old_handler = r->EventHandler;
			handlers[r->EventType].old_context = r->EventContext;

			r->EventHandler = pfw_handler;
			r->EventContext = &handlers[r->EventType];
		}
		else 
		{
			handlers[r->EventType].old_handler = NULL;
			handlers[r->EventType].old_context = NULL;
		}
	}

	return DispatchToLowerDevice( DeviceObject, Irp );
}

////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS TdiEventConnect(
		IN PVOID TdiEventContext,
		IN LONG RemoteAddressLength,
		IN PVOID RemoteAddress,
		IN LONG UserDataLength,
		IN PVOID UserData,
		IN LONG OptionsLength,
		IN PVOID Options,
		OUT CONNECTION_CONTEXT *ConnectionContext,
		OUT PIRP *AcceptIrp)
{
	DBGPRINT("==>TdiEventConnect()!");
	NTSTATUS status;
	PFW_EVENT_CONTEXT *ctx = (PFW_EVENT_CONTEXT*)TdiEventContext;

	TA_ADDRESS *remote_addr = ((TRANSPORT_ADDRESS*)RemoteAddress)->Address;
	sockaddr_in *remote_saddr = (sockaddr_in*)(&remote_addr->AddressType);

	HANDLE pid = PsGetCurrentProcessId();
	ULONG ip = 0;
	USHORT port = 0;
	status = g_Objects.GetInfo( ctx->fileobj, &pid, &ip, &port );
	DBGPRINTVAR(status);

	PACKET_INFO pi;
	pi.pid = pid;
	pi.type = RT_TCP | RT_DIRECTIN;
	pi.ip = ntohl( remote_saddr->sin_addr.s_addr );
	pi.lport = port;
	pi.rport = ntohs(remote_saddr->sin_port);
	DBGPRINT_PACKET_INFO(pi);

	// TODO: 加入过滤代码
	USHORT action = Filter(&pi);
	// TODO: 加入记录日志代码
	status = Log( action, &pi );
	DBGPRINTVART( "Log(action,&pi)", status );

	DBGPRINTVAR(action);
	if ( IS_ACTION_PASS(action) )
	{
		PTDI_IND_CONNECT EventConnect = (PTDI_IND_CONNECT)ctx->old_handler;
		status = EventConnect(ctx->old_context, 
						RemoteAddressLength, RemoteAddress,
						UserDataLength, UserData, OptionsLength, Options, 
						ConnectionContext, AcceptIrp);
	}
	else
	{
		status = STATUS_CONNECTION_REFUSED;
	}

	return status;
}
NTSTATUS TdiEventReceive(
		IN PVOID  TdiEventContext,
		IN CONNECTION_CONTEXT  ConnectionContext,
		IN ULONG  ReceiveFlags,
		IN ULONG  BytesIndicated,
		IN ULONG  BytesAvailable,
		OUT ULONG  *BytesTaken,
		IN PVOID  Tsdu,
		OUT PIRP  *IoRequestPacket)
{
	DBGPRINT("==>TdiEventReceive()!");
	NTSTATUS status;
	PFW_EVENT_CONTEXT *ctx = (PFW_EVENT_CONTEXT*)TdiEventContext;

	USHORT action = ACTION_PASS;

	DBGPRINTVAR(action);
	if ( IS_ACTION_PASS(action) )
	{
		PTDI_IND_RECEIVE EventReceive = (PTDI_IND_RECEIVE)ctx->old_handler;
		status = EventReceive(ctx->old_context, 
					ConnectionContext, ReceiveFlags, 
					BytesIndicated, BytesAvailable, BytesTaken,
					Tsdu, IoRequestPacket);
	}
	else
	{
		status = STATUS_DATA_NOT_ACCEPTED;
	}

	return status;
}

NTSTATUS TdiEventReceiveDatagram(
		IN PVOID TdiEventContext,
		IN LONG SourceAddressLength,
		IN PVOID SourceAddress,
		IN LONG OptionsLength,
		IN PVOID Options,
		IN ULONG ReceiveDatagramFlags,
		IN ULONG BytesIndicated,
		IN ULONG BytesAvailable,
		OUT ULONG *BytesTaken,
		IN PVOID Tsdu,
		OUT PIRP *IoRequestPacket)
{
	DBGPRINT("==>TdiEventReceiveDatagram()!");
	NTSTATUS status;
	PFW_EVENT_CONTEXT *ctx = (PFW_EVENT_CONTEXT*)TdiEventContext;

	TA_ADDRESS *remote_addr = ((TRANSPORT_ADDRESS*)SourceAddress)->Address;
	sockaddr_in *remote_saddr = (sockaddr_in*)(&remote_addr->AddressType);

	HANDLE pid = PsGetCurrentProcessId();
	ULONG ip = 0;
	USHORT port = 0;
	status = g_Objects.GetInfo( ctx->fileobj, &pid, &ip, &port );
	DBGPRINTVAR(status);

	PACKET_INFO pi;
	pi.pid = pid;
	pi.type = RT_UDP | RT_DIRECTIN;
	pi.ip = ntohl( remote_saddr->sin_addr.s_addr );
	pi.lport = port;
	pi.rport = ntohs(remote_saddr->sin_port);
	DBGPRINT_PACKET_INFO(pi);

	// TODO: 加入过滤代码
	USHORT action = Filter(&pi);
	// TODO: 加入记录日志代码
	status = Log( action, &pi );
	DBGPRINTVART( "Log(action,&pi)", status );

	DBGPRINTVAR(action);
	if ( IS_ACTION_PASS(action) )
	{
		PTDI_IND_RECEIVE_DATAGRAM EventReceiveDatagram
			= (PTDI_IND_RECEIVE_DATAGRAM)ctx->old_handler;
		status = EventReceiveDatagram(ctx->old_context, 
				SourceAddressLength, SourceAddress, OptionsLength,
				Options, ReceiveDatagramFlags, 
				BytesIndicated, BytesAvailable, BytesTaken,
				Tsdu, IoRequestPacket);
	}
	else
	{
		status = STATUS_DATA_NOT_ACCEPTED;
	}

	return status;
}






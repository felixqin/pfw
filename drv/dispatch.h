#pragma once


// maximum length of TDI_ADDRESS_TYPE_*
#define TDI_ADDRESS_MAX_LENGTH	TDI_ADDRESS_LENGTH_OSI_TSAP
#define TA_ADDRESS_MAX			(sizeof(TA_ADDRESS) - 1 + TDI_ADDRESS_MAX_LENGTH)
#define TDI_ADDRESS_INFO_MAX	(sizeof(TDI_ADDRESS_INFO) - 1 + TDI_ADDRESS_MAX_LENGTH)


//////////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS TdiDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS TdiDispatchInternalDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS TdiDispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

//////////////////////////////////////////////////////////////////////////////////////////////
//向底层设备转发 IRP 的相关数据结构和函数

typedef struct _TDI_SKIP_CONTEXT
{
	ULONG					NodeSize;
 	PVOID					UserContext;
   PIO_COMPLETION_ROUTINE	CompletionRoutine;			/* old (original) completion routine */
    PVOID					Context;		/* old (original) parameter for old_cr */
	UCHAR					Control;	/* old (original) irps->Control */
} TDI_SKIP_CONTEXT, *PTDI_SKIP_CONTEXT;


NTSTATUS TdiNormalComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context);
NTSTATUS TdiSkipComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context);

NTSTATUS DispatchToLowerDevice(IN PDEVICE_OBJECT LowerDeviceObject, IN PIRP Irp, 
					PIO_COMPLETION_ROUTINE NormalCompleteRoutine = TdiNormalComplete, 
					PIO_COMPLETION_ROUTINE SkipCompleteRoutine = TdiSkipComplete, 
					PVOID UserContext = NULL );


//////////////////////////////////////////////////////////////////////////////////////////////

// 以下函数在 disp_crt.cpp 中实现
NTSTATUS TdiDispatchCreate( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );
NTSTATUS TdiDispatchCleanup( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );

NTSTATUS TdiDispatchAssociate( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );
NTSTATUS TdiDispatchDisassociate( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );

//
NTSTATUS TdiDispatchConnect( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );
NTSTATUS TdiDispatchSendDatagram( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );


//////////////////////////////////////////////////////////////////////////////////////////////
//	IoControl
NTSTATUS TdiDispatchFInsert( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchFDelete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchFModify( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchFMoveDown( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );

NTSTATUS TdiDispatchAInsert( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchADelete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchAModify( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );

NTSTATUS TdiDispatchPfwEnable( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );

NTSTATUS TdiDispatchNotifyGetInfo( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchNotifyStart( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS TdiDispatchNotifyStop( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );

NTSTATUS TdiDispatchClearAllRules( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );



//////////////////////////////////////////////////////////////////////////////////////////////
//	事件函数钩子

NTSTATUS TdiDispatchSetEvent( IN PDEVICE_OBJECT  DeviceObject, IN PIRP  Irp );

NTSTATUS TdiEventConnect(
		IN PVOID TdiEventContext,
		IN LONG RemoteAddressLength,
		IN PVOID RemoteAddress,
		IN LONG UserDataLength,
		IN PVOID UserData,
		IN LONG OptionsLength,
		IN PVOID Options,
		OUT CONNECTION_CONTEXT *ConnectionContext,
		OUT PIRP *AcceptIrp);

NTSTATUS TdiEventReceive(
		IN PVOID  TdiEventContext,
		IN CONNECTION_CONTEXT  ConnectionContext,
		IN ULONG  ReceiveFlags,
		IN ULONG  BytesIndicated,
		IN ULONG  BytesAvailable,
		OUT ULONG  *BytesTaken,
		IN PVOID  Tsdu,
		OUT PIRP  *IoRequestPacket);

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
    OUT PIRP *IoRequestPacket);


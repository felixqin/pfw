#include "precomp.h"
#include "global.h"


NTSTATUS TdiDispatchClearAllRules( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	NTSTATUS status;
	g_AppRules.Clear();
	g_FilterRules.Clear();
	status = Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);	//Ê²Ã´ÒâË¼?
	return status;
}

NTSTATUS TdiDispatchFInsert( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	PCHAR pData = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
	LONG *pPos = (LONG*)pData;
	FILTER_RULE *pRule = (FILTER_RULE*)( pData + sizeof(LONG) );

	NTSTATUS status = g_FilterRules.Insert( pRule, *pPos );

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchFDelete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	LONG *pPos = (LONG*)Irp->AssociatedIrp.SystemBuffer;

	NTSTATUS status = g_FilterRules.Delete( *pPos );
	
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchFModify( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	PCHAR pData = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
	LONG *pPos = (LONG*)pData;
	FILTER_RULE *pRule = (FILTER_RULE*)( pData + sizeof(LONG) );

	NTSTATUS status = g_FilterRules.Modify( pRule, *pPos );

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchFMoveDown( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	LONG *pPos = (LONG*)Irp->AssociatedIrp.SystemBuffer;

	NTSTATUS status = g_FilterRules.MoveDown( *pPos );

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchAInsert( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	APP_RULE *pRule = (APP_RULE*)Irp->AssociatedIrp.SystemBuffer;

	NTSTATUS status = g_AppRules.Insert( pRule );

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchADelete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	HANDLE *ppid = (HANDLE*)Irp->AssociatedIrp.SystemBuffer;

	NTSTATUS status = g_AppRules.Delete( *ppid );

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchAModify( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	APP_RULE *pRule = (APP_RULE*)Irp->AssociatedIrp.SystemBuffer;

	NTSTATUS status = g_AppRules.Modify( pRule );

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchPfwEnable( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	ULONG *pEnable = (ULONG*)Irp->AssociatedIrp.SystemBuffer;
	g_PfwEnabled = *pEnable;
	DBGPRINTVAR(g_PfwEnabled);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS TdiDispatchNotifyGetInfo( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	ASSERT( DeviceObject == g_ControlDevice );

	NTSTATUS status;
	PIO_STACK_LOCATION irps = IoGetCurrentIrpStackLocation(Irp);

	USHORT size = g_Notify.FrontInfoSize();
	Irp->IoStatus.Information = size;

	DBGPRINTVART("TdiDispatchNotifyGetInfo():",size);
	DBGPRINTVAR(irps->Parameters.DeviceIoControl.OutputBufferLength);
	DBGPRINTVAR(Irp->AssociatedIrp.SystemBuffer);

	if (irps->Parameters.DeviceIoControl.OutputBufferLength < size
			|| Irp->AssociatedIrp.SystemBuffer == NULL)
	{
		status = STATUS_MORE_ENTRIES;
	}
	else
	{
		status = g_Notify.FetchInfo( (PNOTIFICATION_INFO)Irp->AssociatedIrp.SystemBuffer );
	}
	DBGPRINTVAR(status);
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchNotifyStart( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	NTSTATUS status = g_Notify.Start();
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
NTSTATUS TdiDispatchNotifyStop( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	NTSTATUS status = g_Notify.Stop();
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


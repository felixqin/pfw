#include "precomp.h"
#include "dispatch.h"
#include "device.h"
#include "global.h"
#include "../public/ioctl.h"

NTSTATUS TdiDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	//DBGPRINT("==>MonitorDispatch enter\n");
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);

	if ( DeviceObject == g_ControlDevice )
	{
		switch(irps->MajorFunction)
		{
		case IRP_MJ_DEVICE_CONTROL:
			DBGPRINT("-->Dispatch(IRP_MJ_DEVICE_CONTROL)...\n");
			status = TdiDispatchDeviceControl(DeviceObject, Irp);
			break;
		//case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		//	DBGPRINT("-->Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL)...\n");
		//	break;
		default:
			DBGPRINT("Dispatch(OTHER_MAJOR_FUNCTION)...\n");
			status = Irp->IoStatus.Status = STATUS_SUCCESS;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
			break;
		}
	}
	else
	{
		switch(irps->MajorFunction)
		{
		case IRP_MJ_CREATE:
			status = TdiDispatchCreate( DeviceObject, Irp);
			DBGPRINT("<--TdiDispatchCreate");
			break;
		//case IRP_MJ_CLOSE:
		//	DBGPRINT("Dispatch(IRP_MJ_CLOSE)...\n");
		//	status = DispatchToLowerDevice( DeviceObject, Irp);
		//	break;
		case IRP_MJ_CLEANUP:
			status = TdiDispatchCleanup( DeviceObject, Irp);
			break;
		case IRP_MJ_DEVICE_CONTROL:
			DBGPRINT("-->Dispatch(IRP_MJ_DEVICE_CONTROL)...\n");
			if (KeGetCurrentIrql() == PASSIVE_LEVEL
				&& NT_SUCCESS(TdiMapUserRequest(DeviceObject, Irp, irps)))
			{
				status = TdiDispatchInternalDeviceControl(DeviceObject, Irp);
			}
			else
			{
				status = DispatchToLowerDevice( DeviceObject, Irp);
			}
			break;
		case IRP_MJ_INTERNAL_DEVICE_CONTROL:
			status = TdiDispatchInternalDeviceControl(DeviceObject,Irp);
			break;
		default:
			DBGPRINT("Dispatch(OTHER_MAJOR_FUNCTION)...\n");
			status = DispatchToLowerDevice( DeviceObject, Irp);
			break;
		}
	}
	return status;
}

NTSTATUS TdiDispatchInternalDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DBGPRINT("==>ControlDispatch enter");

	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	ASSERT(irps->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);
	NTSTATUS status;

	switch (irps->MinorFunction) 
	{
	//case TDI_ACCEPT:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_ACCEPT])...\n");
	//	break;
	//case TDI_ACTION:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_ACTION])...\n");
	//	break;
	case TDI_ASSOCIATE_ADDRESS:
		status = TdiDispatchAssociate( DeviceObject, Irp );
		break;
	case TDI_DISASSOCIATE_ADDRESS:
		status = TdiDispatchDisassociate( DeviceObject, Irp );
		break;
	case TDI_CONNECT:
		DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_CONNECT])...");
		status = TdiDispatchConnect( DeviceObject, Irp );
		break;
	//case TDI_DISCONNECT:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_DISCONNECT])...\n");
	//	break;
	//case TDI_LISTEN:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_LISTEN])...\n");
	//	break;
	//case TDI_QUERY_INFORMATION:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_QUERY_INFORMATION])...\n");
	//	break;
	//case TDI_RECEIVE:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_RECEIVE])...\n");
	//	break;
	//case TDI_RECEIVE_DATAGRAM:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_RECEIVE_DATAGRAM])...\n");
	//	break;
	//case TDI_SEND:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_SEND])...\n");
	//	status = TdiDispatchSend( DeviceObject, Irp );
	//	break;
	case TDI_SEND_DATAGRAM:
		DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_SEND_DATAGRAM])...\n");
		status = TdiDispatchSendDatagram( DeviceObject, Irp );
		break;
	case TDI_SET_EVENT_HANDLER:
		DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[TDI_SET_EVENT_HANDLER])...");
		status = TdiDispatchSetEvent( DeviceObject, Irp );
		break;
	//case TDI_SET_INFORMATION:
	//	DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL\
	//			 [TDI_SET_INFORMATION])...\n");
	//	break;
	default:
		DBGPRINT("Dispatch(IRP_MJ_INTERNAL_DEVICE_CONTROL[INVALID_MINOR_FUNCTION])...");
		status = DispatchToLowerDevice( DeviceObject, Irp);
		break;
	}

	return status;
}

//分发对 pfw_Control 设备的控制命令
NTSTATUS TdiDispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DBGPRINT("==>IoCtlDispatch enter\n");
	ASSERT( DeviceObject == g_ControlDevice );

	PIO_STACK_LOCATION	irps = IoGetCurrentIrpStackLocation(Irp);
	ASSERT(irps->MajorFunction == IRP_MJ_DEVICE_CONTROL);

	NTSTATUS status = STATUS_SUCCESS;
	switch ( irps->Parameters.DeviceIoControl.IoControlCode )
	{
	case IOCTL_CMD_CLEAR_ALL_RULES:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_CLEAR_ALL_RULES])...");
		status =TdiDispatchClearAllRules( DeviceObject, Irp );
		break;
	case IOCTL_CMD_FRULE_INSERT:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_FRULE_INSERT])...");
		status =TdiDispatchFInsert( DeviceObject, Irp );
		break;
	case IOCTL_CMD_FRULE_DELETE:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_FRULE_DELETE])...");
		status =TdiDispatchFDelete( DeviceObject, Irp );
		break;
	case IOCTL_CMD_FRULE_MODIFY:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_FRULE_MODIFY])...");
		status =TdiDispatchFModify( DeviceObject, Irp );
		break;
	case IOCTL_CMD_FRULE_MOVEDOWN:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_FRULE_MOVEDOWN])...");
		status =TdiDispatchFMoveDown( DeviceObject, Irp );
		break;
	case IOCTL_CMD_ARULE_INSERT:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_ARULE_INSERT])...");
		status =TdiDispatchAInsert( DeviceObject, Irp );
		break;
	case IOCTL_CMD_ARULE_DELETE:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_ARULE_DELETE])...");
		status =TdiDispatchADelete( DeviceObject, Irp );
		break;
	case IOCTL_CMD_ARULE_MODIFY:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_ARULE_MODIFY])...");
		status =TdiDispatchAModify( DeviceObject, Irp );
		break;
	case IOCTL_CMD_PFW_ENABLE:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_PFW_ENABLE])...");
		status =TdiDispatchPfwEnable( DeviceObject, Irp );
		break;
	case IOCTL_CMD_NOTIFY_GETINFO:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_NOTIFY_GETINFO])...");
		status = TdiDispatchNotifyGetInfo( DeviceObject, Irp );
		break;
	case IOCTL_CMD_NOTIFY_START:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_NOTIFY_START])...");
		status = TdiDispatchNotifyStart( DeviceObject, Irp );
		break;
	case IOCTL_CMD_NOTIFY_STOP:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[IOCTL_CMD_NOTIFY_STOP])...");
		status = TdiDispatchNotifyStop( DeviceObject, Irp );
		break;
	default:
		DBGPRINT("Dispatch(IRP_MJ_DEVICE_CONTROL[OTHER_COMMAND])...");
		status = Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		break;
	}

	return status;
}


NTSTATUS DispatchToLowerDevice(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp,
					PIO_COMPLETION_ROUTINE NormalCompleteRoutine, 
					PIO_COMPLETION_ROUTINE SkipCompleteRoutine, 
					PVOID UserContext )
{
	PDEVICE_OBJECT LowerDeviceObject 
		= ((PTDIH_DEVICE_EXTENSION) DeviceObject->DeviceExtension)->LowerDeviceObject;

	if ( Irp->CurrentLocation <=1 )
	{
		PIO_STACK_LOCATION	IrpStack = IoGetCurrentIrpStackLocation(Irp);
		IoSkipCurrentIrpStackLocation(Irp);

		PTDI_SKIP_CONTEXT ctx
			= (PTDI_SKIP_CONTEXT)ExAllocatePool(NonPagedPool,sizeof(*ctx));
		ctx->NodeSize = sizeof (TDI_SKIP_CONTEXT);
		if (ctx == NULL)
		{
			DBGPRINT(("tdi_send_irp_to_old_driver: malloc_np\n"));

			NTSTATUS status = Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);

			return status;
		}

		ctx->UserContext = UserContext;
		ctx->CompletionRoutine = IrpStack->CompletionRoutine;
		ctx->Context = IrpStack->Context;
		ctx->Control = IoGetNextIrpStackLocation(Irp)->Control;
		IoSetCompletionRoutine(Irp,SkipCompleteRoutine,ctx,TRUE,TRUE,TRUE);
	}
	else
	{
		IoCopyCurrentIrpStackLocationToNext(Irp);
		IoSetCompletionRoutine(Irp,NormalCompleteRoutine,UserContext,TRUE,TRUE,TRUE);
	}
	//DBGPRINT("==>[DispatchToLowerDevice]IoCallDriver(LowerDeviceObject, Irp)");
	NTSTATUS status = IoCallDriver(LowerDeviceObject, Irp);
	//DBGPRINT("<--[DispatchToLowerDevice]IoCallDriver(LowerDeviceObject, Irp)");
	return status;
}


NTSTATUS TdiNormalComplete(IN	PDEVICE_OBJECT	DeviceObject,
						   IN	PIRP			Irp,
						   IN	PVOID			Context)
{
	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	return STATUS_SUCCESS;
}


NTSTATUS TdiSkipComplete(IN	PDEVICE_OBJECT	DeviceObject,
						IN	PIRP			Irp,
						IN	PVOID			Context)
{
#ifdef _DEBUG
	if (Irp->IoStatus.Status != STATUS_SUCCESS)
		DBGPRINT("tdi_skip_complete: not success status");
#endif
	// restore IRP for using in our completion

	PTDI_SKIP_CONTEXT ctx = (PTDI_SKIP_CONTEXT)Context;
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION IrpStack = IoGetNextIrpStackLocation(Irp);

	ASSERT( DeviceObject == IrpStack->DeviceObject );

	// restore routine and context
	IrpStack->CompletionRoutine = ctx->CompletionRoutine;
	IrpStack->Context = ctx->Context;

	if (ctx->CompletionRoutine != NULL)
	{
		// call old completion (see the old control)
		BOOLEAN b_call = FALSE;

		if (Irp->Cancel)
		{
			// cancel
			b_call = (ctx->Control & SL_INVOKE_ON_CANCEL);
		}
		else 
		{
			if (Irp->IoStatus.Status >= STATUS_SUCCESS)
			{	// success
				b_call = (ctx->Control & SL_INVOKE_ON_SUCCESS);
			}
			else 
			{	// error
				b_call = (ctx->Control & SL_INVOKE_ON_ERROR);
			}
		}

		if (b_call)
			status = ctx->CompletionRoutine(DeviceObject, Irp, ctx->Context);
	}

	ExFreePool(ctx);

	return status;

}


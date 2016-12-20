#include "precomp.h"
#include "device.h"


NTSTATUS create_attach_device(IN PDRIVER_OBJECT DriverObject,
			IN WCHAR *SysDeviceName,
			IN WCHAR *MyDeviceName,
			OUT PDEVICE_OBJECT	*pMyDevice)
{
	DBGPRINT("==>create_attach_device enter.\n");

	NTSTATUS status = 0;
	UNICODE_STRING	DeviceName;

	//得到系统设备对象

	RtlInitUnicodeString( &DeviceName, SysDeviceName );

	PFILE_OBJECT	pfoTarget	= NULL;
	PDEVICE_OBJECT	pdoTarget	= NULL;
	status = IoGetDeviceObjectPointer( IN	&DeviceName,
					IN	FILE_READ_ATTRIBUTES,
					OUT	&pfoTarget,   
					OUT	&pdoTarget);

	if( !NT_SUCCESS(status) )
	{
		DBGPRINT(("create_attach_device: Couldn't get Device Object"));
		*pMyDevice = NULL;
		return( status );
	}

	//创建用户自定义设备对象

	RtlInitUnicodeString( &DeviceName, MyDeviceName );

	status = IoCreateDevice(IN	DriverObject,
							IN	sizeof(TDIH_DEVICE_EXTENSION),
							IN	&DeviceName,
							IN	pdoTarget->DeviceType,
							IN	pdoTarget->Characteristics,
							IN	FALSE,                 
							OUT	pMyDevice);

	if( !NT_SUCCESS(status) )
	{
		DBGPRINT(("create_attach_device: Couldn't create the TCP Filter Device Object\n"));
		ObDereferenceObject( pfoTarget );
		*pMyDevice = NULL;
		return( status );
	}

	//挂接自定义设备对象

	PDEVICE_OBJECT	pdoLower
		= IoAttachDeviceToDeviceStack( *pMyDevice, pdoTarget );
	if( pdoLower == NULL )
	{
		DBGPRINT(("create_attach_device: Couldn't attach to TCP Device Object\n"));
		IoDeleteDevice( *pMyDevice );
		*pMyDevice = NULL;
		ObDereferenceObject( pfoTarget );
		return( status );
	}

	//设备自定义设备对象的属性

	(*pMyDevice)->Flags |= pdoTarget->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO);

	PTDIH_DEVICE_EXTENSION pde = (PTDIH_DEVICE_EXTENSION)(*pMyDevice)->DeviceExtension;
	pde->NodeSize = sizeof(TDIH_DEVICE_EXTENSION);
	pde->TargetDeviceObject = pdoTarget;
	pde->TargetFileObject = pfoTarget;
	pde->LowerDeviceObject = pdoLower;

	DBGPRINT("<==create_attach_device compelte.");

	return status;
}

VOID delete_detach_device( IN PDRIVER_OBJECT DriverObject,
			IN PDEVICE_OBJECT MyDevice)
{
	DBGPRINT("==>delete_detach_device enter.\n");
	if ( MyDevice == NULL ) return;

	PTDIH_DEVICE_EXTENSION pde = (PTDIH_DEVICE_EXTENSION)(MyDevice)->DeviceExtension;

	ASSERT(( pde->TargetDeviceObject != NULL && MyDevice != NULL) );

	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = pde->TargetDeviceObject->DriverObject->MajorFunction[i];

	IoDetachDevice( pde->TargetDeviceObject);
	if( pde->TargetFileObject )
		ObDereferenceObject( pde->TargetFileObject );
	IoDeleteDevice(MyDevice);
}

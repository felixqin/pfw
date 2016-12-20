#pragma once



typedef struct _TDIH_DEVICE_EXTENSION
{
	ULONG			NodeSize;				// 这个结构的大小
	PDEVICE_OBJECT	TargetDeviceObject;		// 绑定的设备对象
	PFILE_OBJECT    TargetFileObject;		// 绑定设备的文件对象
	PDEVICE_OBJECT  LowerDeviceObject;		// 绑定前底层设备对象

}	TDIH_DEVICE_EXTENSION, *PTDIH_DEVICE_EXTENSION;


NTSTATUS
create_attach_device(
		IN PDRIVER_OBJECT DriverObject,
		IN WCHAR *SysDeviceName,
		IN WCHAR *MyDeviceName,
		OUT PDEVICE_OBJECT	*pMyDevice);


VOID
delete_detach_device(
		IN PDRIVER_OBJECT DriverObject,
		IN PDEVICE_OBJECT MyDevice);


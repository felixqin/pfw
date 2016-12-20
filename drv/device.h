#pragma once



typedef struct _TDIH_DEVICE_EXTENSION
{
	ULONG			NodeSize;				// ����ṹ�Ĵ�С
	PDEVICE_OBJECT	TargetDeviceObject;		// �󶨵��豸����
	PFILE_OBJECT    TargetFileObject;		// ���豸���ļ�����
	PDEVICE_OBJECT  LowerDeviceObject;		// ��ǰ�ײ��豸����

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


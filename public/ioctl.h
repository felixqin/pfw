#pragma once


//----------------------------------------------------------------------------------

#define FILE_DEVICE_TDI_PFW		0x1234


#define	IOCTL_CMD_FRULE_INSERT		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x801, METHOD_BUFFERED, FILE_READ_DATA)
#define	IOCTL_CMD_FRULE_DELETE		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x802, METHOD_BUFFERED, FILE_READ_DATA)
#define	IOCTL_CMD_FRULE_MODIFY		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x803, METHOD_BUFFERED, FILE_READ_DATA)
#define	IOCTL_CMD_FRULE_MOVEDOWN		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x804, METHOD_BUFFERED, FILE_READ_DATA)


#define	IOCTL_CMD_ARULE_INSERT		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x811, METHOD_BUFFERED, FILE_READ_DATA)
#define	IOCTL_CMD_ARULE_DELETE		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x812, METHOD_BUFFERED, FILE_READ_DATA)
#define	IOCTL_CMD_ARULE_MODIFY		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x813, METHOD_BUFFERED, FILE_READ_DATA)


#define	IOCTL_CMD_PFW_ENABLE		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x821, METHOD_BUFFERED, FILE_READ_DATA)


#define	IOCTL_CMD_NOTIFY_GETINFO		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x831, METHOD_BUFFERED, FILE_WRITE_DATA)
#define	IOCTL_CMD_NOTIFY_START		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x832, METHOD_NEITHER, 0)
#define	IOCTL_CMD_NOTIFY_STOP		\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x833, METHOD_NEITHER, 0)


#define IOCTL_CMD_CLEAR_ALL_RULES	\
				CTL_CODE(FILE_DEVICE_TDI_PFW, 0x841, METHOD_NEITHER, 0)

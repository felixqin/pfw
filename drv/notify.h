#pragma once

#include "../public/log.h"


class CTNLink
{
	NOTIFICATION_INFO	*info;
	CTNLink				*next;

	friend class CTdiNotify;
};

class CTdiNotify
{
//操作
public:
	void Initialize();
	void Uninitialize();

	NTSTATUS Start();
	NTSTATUS Stop();

	//向应用层程序发送通知
	NTSTATUS Notify( ULONG type, PVOID pContext, USHORT size );
	//取得通知信息
	NTSTATUS FetchInfo( PNOTIFICATION_INFO pInfo );

//属性
public:
	//
	USHORT FrontInfoSize();

private:
	PNOTIFICATION_INFO create_notification_info( ULONG type, PVOID pContext, USHORT lSize );

//链表队列操作, 参见《数据结构》(清华版) ADT Queue
private:
	void ini_queue();		//初始化
	void en_queue(CTNLink *node);	//入队
	CTNLink *dl_queue();		//出队
	void clear_queue();		//置空

//私有变量
private:
	CTNLink	*m_front;	//队头
	CTNLink	*m_rear;	//队尾
	ULONG	m_count;	//队长

	PRKEVENT	m_event;
	HANDLE		m_event_handle;
	BOOLEAN		m_enable;	//是否允许向应用层发消息
	KSPIN_LOCK	m_guard;
};

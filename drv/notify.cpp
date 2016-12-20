#include "precomp.h"
#include "notify.h"



void CTdiNotify::Initialize()
{
	ini_queue();

	UNICODE_STRING event_name;
	RtlInitUnicodeString( &event_name, SHARE_EVENT_NAME_FOR_KERNEL );

	m_event = IoCreateSynchronizationEvent( &event_name, &m_event_handle );
	DBGPRINTVAR(m_event);
	if ( NULL == m_event )
	{
		DBGPRINT("m_NotifyEvent can not create. ");
		m_event_handle = NULL;
	}
	else
	{
		KeClearEvent(m_event);
	}

	m_enable = FALSE;	//暂不允许向应用层程序发消息

	KeInitializeSpinLock( &m_guard );
}
void CTdiNotify::Uninitialize()
{
	DBGPRINT("==>CTdiNotify::Uninitialize()");
	Stop();
	if(m_event != NULL)
	{
		NTSTATUS status = ZwClose(m_event_handle);
		if ( NT_SUCCESS(status) )
		{
			m_event = NULL;
			m_event_handle = NULL;
		}
	}
}
NTSTATUS CTdiNotify::Start()
{
	DBGPRINT("==> CTdiNotify::Start");
	NTSTATUS status;

	KIRQL irql;
	KeAcquireSpinLock(&m_guard, &irql);

	m_enable = ( NULL != m_event );
	status = m_enable ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

	KeReleaseSpinLock(&m_guard,irql);
	return status;
}
NTSTATUS CTdiNotify::Stop()
{
	DBGPRINT("==> CTdiNotify::Stop");
	DBGPRINTVAR(m_event);
	NTSTATUS status = STATUS_SUCCESS;

	KIRQL irql;
	KeAcquireSpinLock(&m_guard, &irql);

	m_enable = FALSE;

	KeResetEvent( m_event );
	clear_queue();
	
	KeReleaseSpinLock(&m_guard,irql);
	return status;
}

NTSTATUS CTdiNotify::Notify( ULONG type, PVOID pContext, USHORT lSize )
{
	DBGPRINT("==> CTdiNotify::Notify");

	DBGPRINTVAR(m_event);
	if ( !m_enable || m_count >= 100 )
	{
		DBGPRINTVAR("Do not send the notification!");
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock(&m_guard, &irql);

	DBGPRINTVAR3(type,*(PLONG)pContext,lSize);

	PNOTIFICATION_INFO pInfo = create_notification_info(type, pContext, lSize);
	CTNLink *node = (CTNLink*)ExAllocatePool(NonPagedPool,sizeof(CTNLink));
	if ( NULL == pInfo || NULL == node )
	{
		DBGPRINT("create_notification_info return value is zero!");
		DBGPRINT("Can not allocate memory for CTNlink *node!");
		if ( pInfo ) ExFreePool( pInfo );
		if ( node ) ExFreePool(node);
		status = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		node->info = pInfo;
		en_queue( node );
		KeSetEvent( m_event, 0, FALSE );
		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock(&m_guard, irql);
	DBGPRINT("<== CTdiNotify::Notify");
	return status;
}
NTSTATUS CTdiNotify::FetchInfo( PNOTIFICATION_INFO pInfo )
{
	DBGPRINT("==>CTdiNotify::FetchInfo()");
	ASSERT( pInfo != NULL );

	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock(&m_guard, &irql);

	CTNLink *node = dl_queue();
	if ( NULL == node )
	{
		DBGPRINT("The queue is empty!");
		status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		DBGPRINTVAR( node->info->TotalSize );
		memcpy( pInfo, node->info, node->info->TotalSize );
		ExFreePool( node->info );
		ExFreePool( node );
		status = STATUS_SUCCESS;
	}

	KeReleaseSpinLock(&m_guard, irql);
	DBGPRINT("<==CTdiNotify::FetchInfo()");
	return status;
}

USHORT CTdiNotify::FrontInfoSize()
{
	KIRQL irql;
	KeAcquireSpinLock(&m_guard, &irql);
	USHORT size = 0;
	if ( m_front != NULL )
	{
		size = m_front->info->TotalSize;
	}
	KeReleaseSpinLock(&m_guard, irql);
	return size;
}

void CTdiNotify::ini_queue()		//初始化
{
	m_front = m_rear = NULL;
	m_count = 0;
}
void CTdiNotify::clear_queue()		//置空
{
	CTNLink *p = m_front;
	while (p)
	{
		CTNLink *q = p;
		p = p->next;
		ExFreePool( q->info );
		ExFreePool(	q );
	}
	m_front = m_rear = NULL;
	m_count = 0;
}

void CTdiNotify::en_queue(CTNLink *node)	//入队
{
	ASSERT(node);
	node->next = NULL;
	if ( NULL == m_front )
	{
		m_front = m_rear = node;
	}
	else
	{
		m_rear->next = node;
		m_rear = node;
	}
	m_count++;
	DBGPRINTVART("++",m_count);
}

CTNLink *CTdiNotify::dl_queue()		//出队
{
	if ( NULL == m_front ) return NULL;

	CTNLink *node = m_front;
	m_front = m_front->next;
	if ( NULL == m_front ) m_rear = NULL;
	m_count--;
	DBGPRINTVART("--",m_count);

	return node;
}

PNOTIFICATION_INFO
CTdiNotify::create_notification_info( ULONG type, PVOID pContext, USHORT lSize )
{
	PNOTIFICATION_INFO pInfo;
	USHORT size = sizeof(NOTIFICATION_INFO) + lSize - sizeof(pInfo->Context);
	pInfo = (PNOTIFICATION_INFO) ExAllocatePool( NonPagedPool, size );

	DBGPRINTVART("CTdiNotify::create_notification_info(): ",pInfo);
	if ( pInfo )
	{
		DBGPRINTVAR(pInfo->TotalSize);
		pInfo->Type = type;
		pInfo->TotalSize = size;
		pInfo->ContextSize = lSize;
		memcpy(pInfo->Context, pContext, lSize);
	}

	return pInfo;
}

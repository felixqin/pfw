#include "precomp.h"
#include "obj.h"


void CObjectTbl::Initialize()
{
	m_link = NULL;
	KeInitializeSpinLock( &m_guard );
}

void CObjectTbl::Uninitialize()
{
	Clear();
}
void CObjectTbl::Clear()
{
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );
	OBJECT_TABLE_ENTRY *p = m_link, *q;
	while ( p )
	{
		q = p;
		p = p->next;
		if ( q->handlers ) ExFreePool( q->handlers );
		ExFreePool( q );
	}
	m_link = NULL;
	KeReleaseSpinLock( &m_guard, irql );
}

NTSTATUS CObjectTbl::Append( PFILE_OBJECT fileobj, HANDLE pid, ULONG ip, USHORT port )
{
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );
	
	OBJECT_TABLE_ENTRY *ote = find( fileobj ) ;
	if ( ote )
	{
		ote->pid = pid;
		ote->ip = ip;
		ote->port = port;
		status = STATUS_SUCCESS;
	}
	else
	{
		ote = (OBJECT_TABLE_ENTRY *)ExAllocatePool( NonPagedPool, sizeof(OBJECT_TABLE_ENTRY) );
		if ( NULL == ote )
		{
			DBGPRINT("Object table append failure! insufficient resources!");
			status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else
		{
			ote->fileobj = fileobj;
			ote->pid = pid;
			ote->ip = ip;
			ote->port = port;
			ote->handlers = NULL;
			ote->next = m_link;
			m_link = ote;
			status = STATUS_SUCCESS;
		}
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}

NTSTATUS CObjectTbl::Remove( PFILE_OBJECT fileobj )
{
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	POBJECT_TABLE_ENTRY p, q;
	p = find ( fileobj, &q );
	if ( NULL == p )
	{
		DBGPRINT("File object is not exsited in object table!");
		status = STATUS_INVALID_PARAMETER_1;
	}
	else
	{
		if ( p != m_link )
		{
			q->next = p->next;
		}
		else
		{
			m_link = p->next;
		}
		if ( p->handlers ) ExFreePool(p->handlers);
		ExFreePool( p );
		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}

POBJECT_TABLE_ENTRY CObjectTbl::find( PFILE_OBJECT fileobj, POBJECT_TABLE_ENTRY *pprev )
{
	POBJECT_TABLE_ENTRY p = m_link, prev = NULL;
	while ( p )
	{
		if ( p->fileobj == fileobj ) break;
		prev = p;
		p = p->next;
	}
	if ( pprev ) *pprev = prev;
	return p;
}

NTSTATUS CObjectTbl::GetInfo( PFILE_OBJECT fileobj, PHANDLE ppid, PULONG pip, PUSHORT pport )
{
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	POBJECT_TABLE_ENTRY ote = find( fileobj );
	if ( NULL == ote )
	{
		DBGPRINT("Cannot search for fileobj in object table!");
		status = STATUS_INVALID_PARAMETER_1;
	}
	else
	{
		if(ppid)	*ppid = ote->pid;
		if(pip)		*pip = ote->ip;
		if(pport)	*pport = ote->port;
		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}

/*
设置文件对象 fileobj 的属性
	如果 ppid, pip, pport 为 NULL, 不改变设置
*/
NTSTATUS CObjectTbl::SetInfo( PFILE_OBJECT fileobj, PHANDLE ppid, PULONG pip, PUSHORT pport )
{
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	POBJECT_TABLE_ENTRY ote = find( fileobj );
	if ( NULL == ote )
	{
		status = STATUS_INVALID_PARAMETER_1;
	}
	else
	{
		if (ppid) ote->pid = *ppid;
		if (pip) ote->ip = *pip;
		if (pport) ote->port = *pport;
		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}


PFW_EVENT_CONTEXT * CObjectTbl::GetEventHandlers( PFILE_OBJECT fileobj )
{
	PFW_EVENT_CONTEXT *handlers;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	POBJECT_TABLE_ENTRY ote = find( fileobj );
	if ( NULL == ote )
	{
		handlers = NULL;
	}
	else if ( ote->handlers != NULL )
	{
		handlers = ote->handlers;
	}
	else	//为 ote->handlers 分配空间
	{
		SIZE_T size = sizeof(PFW_EVENT_CONTEXT) * MAX_EVENT;
		ote->handlers = (PFW_EVENT_CONTEXT*) ExAllocatePool( NonPagedPool, size );
		if ( ote->handlers != NULL )	//分配成功
		{
			RtlZeroMemory( ote->handlers, size );
			handlers = ote->handlers;
		}
	}

	KeReleaseSpinLock( &m_guard, irql );
	return handlers;
}

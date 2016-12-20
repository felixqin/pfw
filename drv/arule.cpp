#include "precomp.h"
#include "arule.h"

void CAppRules::Initialize()
{
	m_link = NULL;
	KeInitializeSpinLock( &m_guard );
}
void CAppRules::Uninitialize()
{
	Clear();
}
void CAppRules::Clear()
{
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	CARLink *q, *p = m_link;
	while ( p )
	{
		q = p;
		p = p->next;
		ExFreePool( q->rule );
		ExFreePool( q );
	}
	m_link = NULL;

	KeReleaseSpinLock( &m_guard, irql );
}
NTSTATUS CAppRules::Insert( APP_RULE *pRule )
{
	DBGPRINT("==>CAppRules::Insert");
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	APP_RULE *rule = (APP_RULE*)ExAllocatePool(NonPagedPool,pRule->size);
	if ( NULL == rule )
	{
		DBGPRINT("CAppRules::Insert: no enough resources!");
		status = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		memcpy( rule, pRule, pRule->size );

		CARLink *prev, *p = find( pRule->pid, &prev );
		if ( p != NULL)		//找到, 更换原规则
		{
			ExFreePool(p->rule);
			p->rule = rule;
			status = STATUS_SUCCESS;
		}
		else		//否则, 插入新规则
		{
			CARLink	*node = (CARLink*)ExAllocatePool(NonPagedPool,sizeof(CARLink));
			if ( NULL == node )
			{
				DBGPRINT("CAppRules::Insert: no enough resources!");
				status = STATUS_INSUFFICIENT_RESOURCES;
			}
			else
			{
				node->rule = rule;
				node->next = m_link;
				m_link = node;
				status = STATUS_SUCCESS;
			}
		}
	}
	KeReleaseSpinLock( &m_guard, irql );
	DBGPRINT("<--CAppRules::Insert");
	return status;
}
NTSTATUS CAppRules::Delete( HANDLE pid )
{
	DBGPRINTVART("CAppRules::Delete(pid): ",pid);
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	CARLink *prev, *p = find( pid, &prev );
	if ( NULL == p )
	{
		DBGPRINT("Can not find this pid!");
		status = STATUS_INVALID_PARAMETER_1;
	}
	else
	{
		if ( NULL == prev )
			m_link = p->next;
		else
			prev->next = p->next;

		ExFreePool(p->rule);
		ExFreePool(p);
		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}
NTSTATUS CAppRules::Modify( APP_RULE *pRule )
{
	NTSTATUS status;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	CARLink *p = find( pRule->pid );
	if ( NULL == p )	//没找到
	{
		DBGPRINT("Can not find this rule!");
		status = STATUS_INVALID_PARAMETER_1;
	}
	else		//找到
	{
		APP_RULE *rule = (APP_RULE*)ExAllocatePool(NonPagedPool,pRule->size);
		if ( NULL == rule )
		{
			DBGPRINT("[CAppRules::Modify] INSUFFICIENT_RESOURCES");
			status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else
		{
			memcpy( rule, pRule, pRule->size );
			ExFreePool(p->rule);
			p->rule = rule;
			status = STATUS_SUCCESS;
		}
	}

	KeReleaseSpinLock( &m_guard, irql );
	return status;
}
BOOLEAN CAppRules::Exist( HANDLE pid )
{
	BOOLEAN bExist;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	bExist = ( find( pid ) != NULL );

	KeReleaseSpinLock( &m_guard, irql );
	return bExist;
}
CARLink * CAppRules::find( HANDLE pid, CARLink **pprev )
{
	CARLink *p = m_link, *prev = NULL;
	while ( p )
	{
		if ( p->rule->pid == pid ) break;
		prev = p;
		p = p->next;
	}
	if ( pprev ) *pprev = (p)?prev:NULL;
	return p;
}

/**************************************************************************
根据数据包信息,返回过滤动作
	参数:
		pInfo:	数据包信息
	返回值:
		过滤动作字

	说明:
		如果返回值为 ACTION_CONTINUE , 表示安全规则链中没有与 pInfo 相适应
	的安全规则, 调用者需进一步处理.
**************************************************************************/
USHORT CAppRules::Filter(PACKET_INFO *pInfo)
{
	USHORT action;
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	CARLink *p = find( pInfo->pid );
	if ( NULL == p )
	{
		DBGPRINT("CAppRules::Filter: Can not find this rule!");
		action = ACTION_CONTINUE;	//让调用者进一步处理
	}
	else	//找到进程的网络访问规则
	{
		DBGPRINTVAR(p->rule->allow);
		action = p->rule->action;		//指定不满足条件时的过滤动作
		if ( IS_ALLOWED(p->rule->allow,pInfo->type) )	//是允许的网络操作
		{
			DBGPRINT("IS ALLOWED!");
			USHORT port_type;
			USHORT i1,i2;
			if ( pInfo->type & RT_TCP )		//是TCP协议
			{
				port_type = p->rule->pt_tcp;
				i1 = 0;
				i2 = p->rule->pn_tcp-1;
			}
			else
			{
				port_type = p->rule->pt_udp;
				i1 = p->rule->pn_tcp;
				i2 = p->rule->pn_tcp + p->rule->pn_udp - 1;
			}
			DBGPRINTVAR3(port_type,p->rule->ports[i1],p->rule->ports[i2]);

			BOOLEAN port_allowed = FALSE;
			if ( port_type & PT_RANGE )		//指定了端口范围
			{
				if ( pInfo->lport >= p->rule->ports[i1]
						&& pInfo->lport <= p->rule->ports[i2] )
					port_allowed = TRUE;				//在指定范围内
			}
			else if ( port_type & PT_LIST)		//指定了端口列表
			{
				for ( int i = i1; i <= i2; i++ )
				{
					if ( pInfo->lport == p->rule->ports[i] )
					{
						port_allowed = TRUE;			//在端口列表中找到
						break;
					}
				}
			}

			if ( port_allowed )	action = ACTION_PASS;
		}
	}

	KeReleaseSpinLock( &m_guard, irql );
	return action;
}



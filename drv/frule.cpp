#include "frule.h"


/*
初始化安全过滤规则类的初始化函数
	初始化一个带头结点的单链表
*/
void CFilterRules::Initialize()
{
	m_rules = (CFRLink*)ExAllocatePool( NonPagedPool, sizeof(CFRLink) );
	if (!m_rules)
	{
		m_rules = NULL;
		DBGPRINT("Initialize filter rules link failure!");
	}
	else
	{
		DBGPRINT("Initialize filter rules link SUCCESS!");
		m_rules->next = NULL;
	}
	KeInitializeSpinLock( &m_guard );
}

/*
初始化安全过滤规则类的反初始化函数
	释放带头结点的单链表的内存空间
*/
void CFilterRules::Uninitialize()
{
	Clear();
	ExFreePool(m_rules);
	m_rules = NULL;
}
void CFilterRules::Clear()
{
	KIRQL irql;
	KeAcquireSpinLock( &m_guard, &irql );

	CFRLink *q,*p = m_rules->next;
	while ( p )
	{
		q = p;
		p = p->next;
		ExFreePool(q->rule);
		ExFreePool(q);
	}
	m_rules->next = NULL;

	KeReleaseSpinLock( &m_guard, irql );
}
/*
跳到链表的指定位置
	nPos为-1,返回头结点的指针
	nPos为0,返回头结点后一结点的指针
	...
	nPos小于-1或大于(规则条数-1)返回 NULL
*/
CFRLink *CFilterRules::go( LONG nPos )
{
	CFRLink *p;
	if ( nPos < -1 )
	{
		p = NULL;
	}
	else
	{
		p = m_rules;
		LONG i=-1;
		while ( i < nPos && p != NULL )
		{
			p = p->next;
			i++;
		}
	}
#ifdef _DEBUG
	if ( p == NULL) DBGPRINT( "nPos is invalid!" );
#endif
	return p;
}
/*
在nPos位置后插入一条安全规则
	如果nPos为-1插入到最前

	返回值:
		STATUS_UNSUCCESSFUL:			插入不成功,原链表不发生任何变化
		STATUS_INSUFFICIENT_RESOURCES:	内存资源不足
		STATUS_SUCCESS:					成功插入到规则链中
*/
NTSTATUS CFilterRules::Insert( FILTER_RULE *pRule, LONG nPos )
{
	ASSERT( nPos >= -1 );

	NTSTATUS status;
	KIRQL irql;

	KeAcquireSpinLock( &m_guard, &irql );
	CFRLink *p = go(nPos);
	if ( p == NULL )
	{
		status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		CFRLink *q = (CFRLink *)ExAllocatePool( NonPagedPool, sizeof(CFRLink) );
		FILTER_RULE *rule = (FILTER_RULE*)ExAllocatePool(NonPagedPool,sizeof(FILTER_RULE));
		if ( q == NULL || rule == NULL )
		{
			if (q) ExFreePool(q);
			if (rule) ExFreePool(rule);
			DBGPRINT("No enough memory!");
			status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else
		{
			memcpy( rule, pRule, sizeof(FILTER_RULE) );
			q->rule = rule;
			q->next = p->next;
			p->next = q;
			status = STATUS_SUCCESS;
		}
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}
/*
删除在nPos位置的一条安全规则
	返回值:
		STATUS_UNSUCCESSFUL:			删除不成功,原链表不发生任何变化
		STATUS_SUCCESS:					成功删除一条规则
*/
NTSTATUS CFilterRules::Delete( LONG nPos )
{
	ASSERT( nPos >= 0 );

	NTSTATUS status;
	KIRQL irql;

	KeAcquireSpinLock( &m_guard, &irql );
	if ( m_rules == NULL )
	{
		status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		CFRLink *p,*q;
		p = go( nPos - 1 );
		if ( p == NULL || p->next == NULL )
		{
			status = STATUS_UNSUCCESSFUL;
		}
		else
		{
			q = p->next;
			p->next = q->next;
			ExFreePool(q->rule);
			ExFreePool(q);
			status = STATUS_SUCCESS;
		}
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}
/*
替换在nPos位置的一条安全规则并释放原规则所占存储空间
	返回值:
		STATUS_UNSUCCESSFUL:			不成功,原链表不发生任何变化
		STATUS_SUCCESS:					成功替换一条规则
*/
NTSTATUS CFilterRules::Modify( FILTER_RULE *pRule, LONG nPos )
{
	ASSERT( nPos >= 0 );
	NTSTATUS status;
	KIRQL irql;

	KeAcquireSpinLock( &m_guard, &irql );
	CFRLink *p = go(nPos);
	if ( p == NULL )
	{
		status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		memcpy( p->rule, pRule, sizeof(FILTER_RULE) );
		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}

/*
将在nPos位置的一条安全规则和nPos+1处的规则交换位置
	返回值:
		STATUS_UNSUCCESSFUL:			不成功,原链表不发生任何变化
		STATUS_SUCCESS:					成功交换
	说明:
		如果nPos或nPos+1小于0或大于(规则条数-1)返回 STATUS_SUCCESS;
*/
NTSTATUS CFilterRules::MoveDown( LONG nPos )
{
	ASSERT( nPos >= 0 );
	CFRLink *p,*q,*r;
	NTSTATUS status;
	KIRQL irql;

	KeAcquireSpinLock( &m_guard, &irql );
	p = go( nPos - 1 );
	if ( p == NULL || p->next == NULL || p->next->next == NULL )
	{
        status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		q = p->next;
		r = q->next;
		p->next = r;
		q->next = r->next;
		r->next = q;

		status = STATUS_SUCCESS;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return status;
}
//FILTER_RULE * CFilterRules::First()
//{
//	m_cur = m_rules->next;
//	if ( m_cur == NULL ) return NULL;
//	return m_cur->rule;
//}
//FILTER_RULE * CFilterRules::Next()
//{
//	if ( m_cur != NULL ) m_cur = m_cur->next;
//	if ( m_cur == NULL ) return NULL;
//	return m_cur->rule;
//}

/**************************************************************************
根据数据包信息,返回过滤动作
	参数:
		pInfo:	数据包信息
	返回值:
		过滤动作字

	说明:
		如果返回值为 ACTION_CONTINUE , 表示安全规则链中没有与 pInfo 相适应
	的安全规则, 调用者需进一步过滤.
**************************************************************************/
USHORT CFilterRules::Filter(PACKET_INFO *pInfo)
{
	if ( NULL == m_rules ) return ACTION_CONTINUE;

	USHORT action = ACTION_CONTINUE;
	KIRQL irql;

	KeAcquireSpinLock( &m_guard, &irql );
	CFRLink *p = m_rules->next;
	while ( p )
	{
		//数据方向相同并且ip地址在本规则所包含的范围内
		if ( RT_DIRECT_EQUAL(p->rule->type, pInfo->type )
			&& (p->rule->ip & p->rule->mask) == (pInfo->ip & p->rule->mask) )
		{
			if ( ( p->rule->type & RT_IP ) )			//IP规则不用判断端口
			{
				action = p->rule->action;
				//如果不继续下一条规则, 跳出循环
				if ( !IS_ACTION_CONTINUE(action) ) break;
			}
			else if ( RT_PROTO_EQUAL(p->rule->type, pInfo->type) ) //协议类型是否匹配
			{
				USHORT port
					= (IS_DIRECTIN(p->rule->type)) ? pInfo->lport : pInfo->rport;
				//端口匹配检查. 数据方向为接收,判断本地端口,反之,对方端口
				if ( port >= p->rule->port1 && port <= p->rule->port2 )
				{
					action = p->rule->action;
					//如果不继续下一条规则, 跳出循环
					if ( !IS_ACTION_CONTINUE(action) ) break;
				}
			}
		}
		p = p->next;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return action;
}

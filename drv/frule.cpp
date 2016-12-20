#include "frule.h"


/*
��ʼ����ȫ���˹�����ĳ�ʼ������
	��ʼ��һ����ͷ���ĵ�����
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
��ʼ����ȫ���˹�����ķ���ʼ������
	�ͷŴ�ͷ���ĵ�������ڴ�ռ�
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
���������ָ��λ��
	nPosΪ-1,����ͷ����ָ��
	nPosΪ0,����ͷ����һ����ָ��
	...
	nPosС��-1�����(��������-1)���� NULL
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
��nPosλ�ú����һ����ȫ����
	���nPosΪ-1���뵽��ǰ

	����ֵ:
		STATUS_UNSUCCESSFUL:			���벻�ɹ�,ԭ���������κα仯
		STATUS_INSUFFICIENT_RESOURCES:	�ڴ���Դ����
		STATUS_SUCCESS:					�ɹ����뵽��������
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
ɾ����nPosλ�õ�һ����ȫ����
	����ֵ:
		STATUS_UNSUCCESSFUL:			ɾ�����ɹ�,ԭ���������κα仯
		STATUS_SUCCESS:					�ɹ�ɾ��һ������
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
�滻��nPosλ�õ�һ����ȫ�����ͷ�ԭ������ռ�洢�ռ�
	����ֵ:
		STATUS_UNSUCCESSFUL:			���ɹ�,ԭ���������κα仯
		STATUS_SUCCESS:					�ɹ��滻һ������
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
����nPosλ�õ�һ����ȫ�����nPos+1���Ĺ��򽻻�λ��
	����ֵ:
		STATUS_UNSUCCESSFUL:			���ɹ�,ԭ���������κα仯
		STATUS_SUCCESS:					�ɹ�����
	˵��:
		���nPos��nPos+1С��0�����(��������-1)���� STATUS_SUCCESS;
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
�������ݰ���Ϣ,���ع��˶���
	����:
		pInfo:	���ݰ���Ϣ
	����ֵ:
		���˶�����

	˵��:
		�������ֵΪ ACTION_CONTINUE , ��ʾ��ȫ��������û���� pInfo ����Ӧ
	�İ�ȫ����, ���������һ������.
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
		//���ݷ�����ͬ����ip��ַ�ڱ������������ķ�Χ��
		if ( RT_DIRECT_EQUAL(p->rule->type, pInfo->type )
			&& (p->rule->ip & p->rule->mask) == (pInfo->ip & p->rule->mask) )
		{
			if ( ( p->rule->type & RT_IP ) )			//IP�������ж϶˿�
			{
				action = p->rule->action;
				//�����������һ������, ����ѭ��
				if ( !IS_ACTION_CONTINUE(action) ) break;
			}
			else if ( RT_PROTO_EQUAL(p->rule->type, pInfo->type) ) //Э�������Ƿ�ƥ��
			{
				USHORT port
					= (IS_DIRECTIN(p->rule->type)) ? pInfo->lport : pInfo->rport;
				//�˿�ƥ����. ���ݷ���Ϊ����,�жϱ��ض˿�,��֮,�Է��˿�
				if ( port >= p->rule->port1 && port <= p->rule->port2 )
				{
					action = p->rule->action;
					//�����������һ������, ����ѭ��
					if ( !IS_ACTION_CONTINUE(action) ) break;
				}
			}
		}
		p = p->next;
	}
	KeReleaseSpinLock( &m_guard, irql );
	return action;
}

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
		if ( p != NULL)		//�ҵ�, ����ԭ����
		{
			ExFreePool(p->rule);
			p->rule = rule;
			status = STATUS_SUCCESS;
		}
		else		//����, �����¹���
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
	if ( NULL == p )	//û�ҵ�
	{
		DBGPRINT("Can not find this rule!");
		status = STATUS_INVALID_PARAMETER_1;
	}
	else		//�ҵ�
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
�������ݰ���Ϣ,���ع��˶���
	����:
		pInfo:	���ݰ���Ϣ
	����ֵ:
		���˶�����

	˵��:
		�������ֵΪ ACTION_CONTINUE , ��ʾ��ȫ��������û���� pInfo ����Ӧ
	�İ�ȫ����, ���������һ������.
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
		action = ACTION_CONTINUE;	//�õ����߽�һ������
	}
	else	//�ҵ����̵�������ʹ���
	{
		DBGPRINTVAR(p->rule->allow);
		action = p->rule->action;		//ָ������������ʱ�Ĺ��˶���
		if ( IS_ALLOWED(p->rule->allow,pInfo->type) )	//��������������
		{
			DBGPRINT("IS ALLOWED!");
			USHORT port_type;
			USHORT i1,i2;
			if ( pInfo->type & RT_TCP )		//��TCPЭ��
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
			if ( port_type & PT_RANGE )		//ָ���˶˿ڷ�Χ
			{
				if ( pInfo->lport >= p->rule->ports[i1]
						&& pInfo->lport <= p->rule->ports[i2] )
					port_allowed = TRUE;				//��ָ����Χ��
			}
			else if ( port_type & PT_LIST)		//ָ���˶˿��б�
			{
				for ( int i = i1; i <= i2; i++ )
				{
					if ( pInfo->lport == p->rule->ports[i] )
					{
						port_allowed = TRUE;			//�ڶ˿��б����ҵ�
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



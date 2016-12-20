/*
ģ��3.1 :	��ȫ����ά��
	����:	CFilterRules
	�ļ�:	FRULE.H		FRULE.CPP
*/

#pragma once

#include "precomp.h"
#include "pktinfo.h"
#include "../public/rule.h"

class CFRLink	//filter rule link table
{
	FILTER_RULE *rule;
	CFRLink *next;

	friend class CFilterRules;
};

class CFilterRules
{
//����
public:
	NTSTATUS Insert( FILTER_RULE *pRule, LONG nPos );
	NTSTATUS Delete( LONG nPos );
	NTSTATUS Modify( FILTER_RULE *pRule, LONG nPos );
	NTSTATUS MoveDown( LONG nPos );

	USHORT	Filter(PACKET_INFO *pInfo);		//***�������ݰ���Ϣ,���ع��˶���

public:
	void Initialize();
	void Uninitialize();
	void Clear();

private:
	CFRLink *m_rules;
	KSPIN_LOCK m_guard;

private:
	CFRLink *go( LONG nPos );
};

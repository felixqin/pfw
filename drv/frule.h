/*
模块3.1 :	安全规则维护
	类名:	CFilterRules
	文件:	FRULE.H		FRULE.CPP
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
//方法
public:
	NTSTATUS Insert( FILTER_RULE *pRule, LONG nPos );
	NTSTATUS Delete( LONG nPos );
	NTSTATUS Modify( FILTER_RULE *pRule, LONG nPos );
	NTSTATUS MoveDown( LONG nPos );

	USHORT	Filter(PACKET_INFO *pInfo);		//***根据数据包信息,返回过滤动作

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

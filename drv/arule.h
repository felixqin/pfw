#pragma once

#include "precomp.h"
#include "pktinfo.h"
#include "../public/rule.h"

class CARLink
{
	APP_RULE	*rule;
	CARLink		*next;

	friend class CAppRules;
};


class CAppRules
{
public:
	NTSTATUS Insert( APP_RULE *pRule );
	NTSTATUS Delete( HANDLE pid );
	NTSTATUS Modify( APP_RULE *pRule );

	BOOLEAN Exist( HANDLE pid );

	USHORT	Filter( PACKET_INFO *pInfo );

public:
	void Initialize();
	void Uninitialize();
	void Clear();

private:
	CARLink *m_link;
	KSPIN_LOCK m_guard;

private:
	CARLink *find( HANDLE pid, CARLink **pprev = NULL );
};

#include "precomp.h"
#include "pktinfo.h"
#include "global.h"

USHORT Filter( PACKET_INFO *pInfo )
{
	if ( !g_PfwEnabled )	return ACTION_PASS;

	USHORT action = g_FilterRules.Filter(pInfo);
	USHORT log = GET_ACTION_LOG_INFO(action);
	if ( !IS_ACTION_CONTINUE(action) )	return action;

	action = g_AppRules.Filter(pInfo);
	if ( !IS_ACTION_CONTINUE(action) )	return action;

	//没找到匹配的规则,放行并使用日志通知方式
	return ACTION_PASS | log;
}

NTSTATUS Log( USHORT action, PACKET_INFO *pInfo )
{
	if ( !NEED_LOG(action) ) return STATUS_SUCCESS;

	LOG_INFO li;
	li.pid = pInfo->pid;
	li.type = pInfo->type;
	li.action = action;
	li.ip = pInfo->ip;
	li.rport = pInfo->rport;
	li.lport = pInfo->lport;

	return g_Notify.Notify( NT_LOG, &li, sizeof(li) );
}

#include "stdafx.h"
#include "d_arule.h"


//////////////////////////////////////////////////
class CARLNode
{
private:
	APP_RULE	*rule;
	CARLNode	*next;
public:
	CARLNode(): rule(NULL), next(NULL)	{}
	CARLNode(const APP_RULE &r)
	{
		rule = (APP_RULE*)new BYTE[r.size];
		memcpy( rule, &r, r.size );
		rule->app_name = new TCHAR[ _tcslen(r.app_name) + 1 ];
		_tcscpy( rule->app_name, r.app_name );
		next = NULL;
	}

	~CARLNode()
	{
		if (rule)
		{
			delete[] rule->app_name;
			delete[] rule;
		}
	}

	friend class CAppRules;
};


//////////////////////////////////////////////////////////////
CAppRules::CAppRules()
{
	m_link = new CARLNode;
}
CAppRules::~CAppRules()
{
	while (m_link)
	{
		CARLNode *p = m_link;
		m_link = m_link->next;
		delete p;
	}
}

//////////////////////////////////////////////////////////////
BOOL CAppRules::Insert( const APP_RULE &rule )
{
	if ( find_prev(rule.app_name) != NULL )	//链表中已存在
		return FALSE;

	CARLNode *node = new CARLNode(rule);
	node->next = m_link->next;
	m_link->next = node;
	return TRUE;
}
BOOL CAppRules::Delete( LPCTSTR szAppName )
{
	CARLNode *prev = find_prev(szAppName);
	if ( NULL == prev ) return FALSE;

	CARLNode *p = prev->next;
	prev->next = p->next;
	delete p;

	return TRUE;
}
BOOL CAppRules::Modify( const APP_RULE &rule )
{
	CARLNode *prev = find_prev(rule.app_name);
	if ( NULL == prev ) return FALSE;

	CARLNode *node = new CARLNode(rule);
	CARLNode *p = prev->next;
	prev->next = node;
	node->next = p->next;
	delete p;

	return TRUE;
}

APP_RULE * CAppRules::Find( LPCTSTR szAppName )
{
	CARLNode *prev = find_prev( szAppName );
	if ( NULL == prev ) return NULL;

	return prev->next->rule;
}
//////////////////////////////////////////////////////////////
CARLNode * CAppRules::find_prev( LPCTSTR szAppName )
{
	if ( szAppName == NULL ) return NULL;
	CARLNode *prev = m_link;
	while ( prev->next )
	{
		if ( _tcscmp( szAppName, prev->next->rule->app_name ) == 0 )
			break;

		prev = prev->next;
	}
	if ( prev->next == NULL ) return NULL;

	return prev;
}


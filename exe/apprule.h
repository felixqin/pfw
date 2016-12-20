#pragma once

#include <list>


using namespace std;


class CARule
{
public:
	CARule( LPCTSTR appname)
	{
		ULONG size = sizeof(APP_RULE) + sizeof(USHORT)*4 - sizeof(rule->ports);
		rule = (APP_RULE*)new BYTE[size];
		VERIFY(rule);
		rule->app_name = new TCHAR[_tcslen(appname) + 1];
		_tcscpy( rule->app_name, appname );

		rule->size = size;
		rule->pid = 0;
		rule->allow = ALLOW_ALL;
		rule->action = ACTION_DENY_LOG_NBEEP;
		rule->pt_tcp = rule->pt_udp = PT_RANGE;
		rule->pn_tcp = rule->pn_udp = 2;
		rule->ports[0] = rule->ports[2] = 0;
		rule->ports[1] = rule->ports[3] = 0xFFFF;
	}

	CARule( LPCTSTR appname, USHORT allow, USHORT action, 
		USHORT pt_tcp, USHORT pn_tcp, USHORT tcp_ports[], 
		USHORT pt_udp, USHORT pn_udp, USHORT udp_ports[] )
	{
		ULONG size = sizeof(APP_RULE) + sizeof(USHORT)*( pn_tcp + pn_udp ) - sizeof(rule->ports);
		rule = (APP_RULE*) new BYTE[size];
		VERIFY( rule );
		rule->app_name = new TCHAR[_tcslen(appname) + 1];
		_tcscpy( rule->app_name, appname );

		rule->size = size;
		rule->pid = 0;
		rule->allow = allow;
		rule->action = action;
		rule->pt_tcp = pt_tcp;
		rule->pn_tcp = pn_tcp;
		rule->pt_udp = pt_udp;
		rule->pn_udp = pn_udp;
		memcpy( rule->ports, tcp_ports, sizeof(USHORT)*pn_tcp );
		memcpy( &rule->ports[pn_tcp], udp_ports, sizeof(USHORT)*pn_udp );
	}
	CARule( const APP_RULE &r )
	{
		rule = (APP_RULE*) new BYTE[r.size];
		VERIFY(rule);
		memcpy( rule, &r, r.size );
		rule->app_name = new TCHAR[_tcslen(r.app_name) + 1];
		_tcscpy( rule->app_name, r.app_name );
	}
	CARule( const CARule &r )
	{
		rule = (APP_RULE*) new BYTE[r.rule->size];
		VERIFY(rule);
		memcpy( rule, r.rule, r.rule->size );
		rule->app_name = new TCHAR[_tcslen(r.rule->app_name) + 1];
		_tcscpy( rule->app_name, r.rule->app_name );
	}
	~CARule()
	{
		ASSERT(rule);
		delete[] rule->app_name;
		delete[] rule;
	}
public:
	CARule & operator = ( const CARule& r )
	{
		if ( &r == this ) return *this;	//·ÀÖ¹×Ô¸³Öµ

		delete[] rule->app_name;
		delete[] rule;

		rule = (APP_RULE*) new BYTE[r.rule->size];
		VERIFY(rule);
		memcpy( rule, r.rule, r.rule->size );
		rule->app_name = new TCHAR[_tcslen(r.rule->app_name) + 1];
		_tcscpy( rule->app_name, r.rule->app_name );

		return *this;
	}
	bool operator == ( const CARule& r ) const
	{
		bool ret = ( rule->size == r.rule->size )
			&& ( rule->allow == r.rule->allow )
			&& ( rule->action == r.rule->action )
			&& ( rule->pt_tcp == r.rule->pt_tcp )
			&& ( rule->pt_udp == r.rule->pt_udp )
			&& ( rule->pn_tcp == r.rule->pn_tcp )
			&& ( rule->pn_udp == r.rule->pn_udp )
			&& ( _tcscmp(rule->app_name,r.rule->app_name) == 0 )
			&& ( memcmp(rule->ports,r.rule->ports,
					rule->pn_tcp + rule->pn_udp ) == 0 );
		return ret;
	}
	const APP_RULE* GetRule() const
	{
		return rule;
	}
private:
	APP_RULE *rule;
};

class CAppRules
{
public:
	CAppRules();

public:
	BOOL Insert(const CARule& r);
	BOOL Delete(LPCTSTR appname);
	BOOL Modify(const CARule& r);

	const APP_RULE* First();
	const APP_RULE* Next();
	const APP_RULE* GetRule( LPCTSTR appname ) const;

	int LoadFromFile(LPCTSTR szFileName);
	int SaveToFile(LPCTSTR szFileName) const;

private:
	list<CARule> m_rules;
	list<CARule>::iterator m_iter;
};

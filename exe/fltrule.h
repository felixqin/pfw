#pragma once

#include "../public/rule.h"
#include <list>

using namespace std;


//���˹�����
class CFRule
{
public:
	CFRule( LPCTSTR name, 
		LPCTSTR description, 
		USHORT type = RT_TCP,
        ULONG ip = 0, ULONG mask = 0,
		USHORT port1 = 0, USHORT port2 = 65535,
		USHORT action = ACTION_CONTINUE | ACTION_LOG_NBEEP )
	{
		rule.name = new TCHAR[_tcslen(name)+1];
		_tcscpy( rule.name, name );

		if (description)
		{
			rule.description = new TCHAR[_tcslen(description)+1];
			_tcscpy( rule.description, description );
		}
		else
		{
			rule.description = new TCHAR[1];
			rule.description[0] = _T('\0');
		}

		rule.type = type;
		rule.ip = ip;
		rule.mask = mask;
		rule.port1 = port1;
		rule.port2 = port2;
		rule.action = action;
	}
	CFRule( const FILTER_RULE &r )
	{
		memcpy( &rule, &r, sizeof(r) );
		rule.name = new TCHAR[_tcslen(r.name)+1];
		_tcscpy( rule.name, r.name );
		rule.description = new TCHAR[_tcslen(r.description)+1];
		_tcscpy( rule.description, r.description );
	}
	CFRule( const CFRule &r )
	{
		memcpy( &rule, &r.rule, sizeof(r) );
		rule.name = new TCHAR[_tcslen(r.rule.name)+1];
		_tcscpy( rule.name, r.rule.name );
		rule.description = new TCHAR[_tcslen(r.rule.description)+1];
		_tcscpy( rule.description, r.rule.description );
	}
	~CFRule()
	{
		delete rule.name;
		delete rule.description;
	}
public:
	CFRule &operator = ( const CFRule &r )
	{
		if ( &r == this ) return *this;	//��ֹ�Ը�ֵ

		delete rule.name;
		delete rule.description;

		memcpy( &rule, &r.rule, sizeof(r.rule) );

		rule.name = new TCHAR[_tcslen(r.rule.name)+1];
		_tcscpy( rule.name, r.rule.name );

		rule.description = new TCHAR[_tcslen(r.rule.description)+1];
		_tcscpy( rule.description, r.rule.description );

		return *this;
	}
	bool operator == ( const CFRule &r)
	{
		bool ret =  (rule.type == r.rule.type)
				&& (rule.ip == r.rule.ip)
				&& (rule.mask == r.rule.mask)
				&& (rule.port1 == r.rule.port1)
				&& (rule.port2 == r.rule.port2)
				&& (rule.action == r.rule.action)
				&& ( _tcscmp(rule.name, r.rule.name) == 0 )
				&& ( _tcscmp(rule.description, r.rule.description) == 0 );
		return ret;
	}
	const FILTER_RULE* GetRule() const
	{
		return &rule;
	}
private:
	FILTER_RULE rule;
};


class CFilterRules
{
public:
	CFilterRules();
public:

	/*
	��ȫ����������
	*/

	//�� pos �����һ����ȫ����
	BOOL InsertAfter( const CFRule &rule, int pos );
	//ɾ�� pos ���Ĺ���
	BOOL Delete( int pos );
	//�滻 pos ���Ĺ���
	BOOL Modify( const CFRule &rule, int pos );

	BOOL MoveUp( int pos );
	BOOL MoveDown( int pos );

	const FILTER_RULE* First();
	const FILTER_RULE* Next();
	const FILTER_RULE* GetRule( int pos ) const;

	/*
	�����ļ���д����
	*/
	int LoadFromFile(LPCTSTR szFileName);
	int SaveToFile(LPCTSTR szFileName) const;

private:
	list<CFRule> m_rules;
	list<CFRule>::iterator m_iter;

};

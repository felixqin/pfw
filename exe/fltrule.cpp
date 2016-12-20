#include "stdafx.h"
#include "resource.h"
#include "fltrule.h"


CFilterRules::CFilterRules()
{
	m_iter = m_rules.begin();
}

//在 pos 后插入一条安全规则
BOOL CFilterRules::InsertAfter( const CFRule &rule, int pos )
{
	ASSERT( pos >= -1 && pos < (int)m_rules.size() );

	if ( !PfwFilterRuleInsert( (FILTER_RULE*)rule.GetRule(), pos ) )
		return FALSE;

	list<CFRule>::size_type size = m_rules.size();
	list<CFRule>::iterator iter = m_rules.begin();

	for ( int i = 0; i < pos+1; i++ ) iter++;
	m_rules.insert( iter, rule );

	return (m_rules.size() - size) == 1;
}
//删除 pos 处的规则
BOOL CFilterRules::Delete( int pos )
{
	ASSERT( pos >= 0 && pos < (int)m_rules.size() );

	if ( !PfwFilterRuleDelete( pos ) )
		return FALSE;

	list<CFRule>::size_type size = m_rules.size();
	list<CFRule>::iterator iter = m_rules.begin();

	for ( int i = 0; i < pos; i++ ) iter++;
	m_rules.erase( iter );

	return (size - m_rules.size()) == 1;
}
//替换 pos 处的规则
BOOL CFilterRules::Modify( const CFRule &rule, int pos )
{
	ASSERT( pos >= 0 && pos < (int)m_rules.size() );

	if ( !PfwFilterRuleModify( (FILTER_RULE*)rule.GetRule(), pos ) )
		return FALSE;

	list<CFRule>::size_type size = m_rules.size();
	list<CFRule>::iterator iter = m_rules.begin();

	for ( int i = 0; i < pos; i++ ) iter++;
	*iter = rule;

	return TRUE;
}
BOOL CFilterRules::MoveDown( int pos )
{
	if ( pos < 0 || pos >= (int)m_rules.size() - 1 )
		return FALSE;

	if ( !PfwFilterRuleMoveDown( pos ) )
		return FALSE;

	list<CFRule>::size_type size = m_rules.size();
	list<CFRule>::iterator iter1 = m_rules.begin();

	for ( int i = 0; i < pos; i++ ) iter1++;
	list<CFRule>::iterator iter2 = iter1;
	iter2++;
	swap(*iter1,*iter2);

    return TRUE;
}
BOOL CFilterRules::MoveUp( int pos )
{
	return MoveDown( pos-1 );
}
const FILTER_RULE* CFilterRules::First()
{
	if ( m_rules.empty() )
		return NULL;

	m_iter = m_rules.begin();
	return m_iter->GetRule();
}
const FILTER_RULE* CFilterRules::Next()
{
	if ( ++m_iter == m_rules.end() )
		return NULL;

	return m_iter->GetRule();
}
const FILTER_RULE* CFilterRules::GetRule( int pos ) const
{
	ASSERT( pos >= -1 && pos < (int)m_rules.size() );

	list<CFRule>::const_iterator iter = m_rules.begin();

	for ( int i = 0; i < pos; i++ ) iter++;

	return iter->GetRule();
}
int CFilterRules::LoadFromFile(LPCTSTR szFileName)
{
	CFile in;

	if ( !in.Open( szFileName, CFile::modeRead 
				| CFile::shareDenyWrite | CFile::typeBinary ) )
	{
		AfxMessageBox( IDS_MB_ERROR_FILEOPEN, MB_OK | MB_ICONERROR );
		return -1;
	}

	DWORD count = 0;
	if ( in.GetLength() != 0 )	//文件非空
	{
		DWORD cc = 0;
		in.Read( &cc, sizeof(cc) );			//读规则个数
		for ( DWORD i=0; i<cc; i++ )
		{
			FILTER_RULE rule;
			in.Read( &rule, sizeof(FILTER_RULE) );			//读结构数据

			DWORD ss;
			in.Read( &ss, sizeof(ss) );		//读规则名称大小
			rule.name = (TCHAR*) new BYTE[ss];
			in.Read( rule.name, ss );		//读规则名称

			in.Read( &ss, sizeof(ss) );		//读规则说明大小
			rule.description = (TCHAR*) new BYTE[ss];
			in.Read( rule.description, ss );	//读规则说明

			if ( InsertAfter(rule, m_rules.size()-1) )
				count++;	//插入此规则并计数
		}
	}
	in.Close();
	return count;
}
int CFilterRules::SaveToFile(LPCTSTR szFileName) const
{
	CFile out;
	
	if ( !out.Open( szFileName, CFile::modeCreate | CFile::modeWrite
					| CFile::shareExclusive | CFile::typeBinary ) )
	{
		AfxMessageBox( IDS_MB_ERROR_FILEOPEN, MB_OK | MB_ICONERROR );
		return -1;
	}

	DWORD cc = (DWORD)m_rules.size();
	out.Write( &cc, sizeof(cc) );	//存规则个数
	/*
	//安全规则结构
	typedef struct _FILTER_RULE
	{
		LPTSTR	name;			//规则名称;	在驱动程序模块中不使用,无意义
		LPTSTR	description;	//规则说明;	在驱动程序模块中不使用,无意义
		USHORT	type;			//规则类型字
		USHORT	action;			//过滤动作字
		ULONG	ip;				//对方IP地址
		ULONG	mask;			//地址掩码
		USHORT	port1;			//起始端口;	当数据包方向为 in, 表示本地端口
		USHORT	port2;			//结束端口;	当数据包方向为 out, 表示对方端口
	} FILTER_RULE, *PFILTER_RULE;
	*/
	list<CFRule>::const_iterator iter = m_rules.begin();
	for ( DWORD i=0; i<cc; i++, iter++ )
	{
		const FILTER_RULE *rule = iter->GetRule();

		out.Write( rule, sizeof(FILTER_RULE) );	//存结构数据

		DWORD ss = sizeof(TCHAR) * ( (DWORD)_tcslen( rule->name ) + 1 );
		out.Write( &ss, sizeof(ss) );	//存规则名称大小
		out.Write( rule->name, ss );	//存规则名称

		ss = sizeof(TCHAR) * ( _tcslen( rule->description) + 1 );
		out.Write( &ss, sizeof(ss) );		//存规则说明大小
		out.Write( rule->description, ss );	//存规则说明
	}

	out.Close();
	return cc;
}


#include "stdafx.h"
#include "resource.h"
#include "fltrule.h"


CFilterRules::CFilterRules()
{
	m_iter = m_rules.begin();
}

//�� pos �����һ����ȫ����
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
//ɾ�� pos ���Ĺ���
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
//�滻 pos ���Ĺ���
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
	if ( in.GetLength() != 0 )	//�ļ��ǿ�
	{
		DWORD cc = 0;
		in.Read( &cc, sizeof(cc) );			//���������
		for ( DWORD i=0; i<cc; i++ )
		{
			FILTER_RULE rule;
			in.Read( &rule, sizeof(FILTER_RULE) );			//���ṹ����

			DWORD ss;
			in.Read( &ss, sizeof(ss) );		//���������ƴ�С
			rule.name = (TCHAR*) new BYTE[ss];
			in.Read( rule.name, ss );		//����������

			in.Read( &ss, sizeof(ss) );		//������˵����С
			rule.description = (TCHAR*) new BYTE[ss];
			in.Read( rule.description, ss );	//������˵��

			if ( InsertAfter(rule, m_rules.size()-1) )
				count++;	//����˹��򲢼���
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
	out.Write( &cc, sizeof(cc) );	//��������
	/*
	//��ȫ����ṹ
	typedef struct _FILTER_RULE
	{
		LPTSTR	name;			//��������;	����������ģ���в�ʹ��,������
		LPTSTR	description;	//����˵��;	����������ģ���в�ʹ��,������
		USHORT	type;			//����������
		USHORT	action;			//���˶�����
		ULONG	ip;				//�Է�IP��ַ
		ULONG	mask;			//��ַ����
		USHORT	port1;			//��ʼ�˿�;	�����ݰ�����Ϊ in, ��ʾ���ض˿�
		USHORT	port2;			//�����˿�;	�����ݰ�����Ϊ out, ��ʾ�Է��˿�
	} FILTER_RULE, *PFILTER_RULE;
	*/
	list<CFRule>::const_iterator iter = m_rules.begin();
	for ( DWORD i=0; i<cc; i++, iter++ )
	{
		const FILTER_RULE *rule = iter->GetRule();

		out.Write( rule, sizeof(FILTER_RULE) );	//��ṹ����

		DWORD ss = sizeof(TCHAR) * ( (DWORD)_tcslen( rule->name ) + 1 );
		out.Write( &ss, sizeof(ss) );	//��������ƴ�С
		out.Write( rule->name, ss );	//���������

		ss = sizeof(TCHAR) * ( _tcslen( rule->description) + 1 );
		out.Write( &ss, sizeof(ss) );		//�����˵����С
		out.Write( rule->description, ss );	//�����˵��
	}

	out.Close();
	return cc;
}


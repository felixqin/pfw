#include "stdafx.h"
#include "apprule.h"
#include "resource.h"


CAppRules::CAppRules()
{
	m_iter = m_rules.begin();
}
BOOL CAppRules::Insert(const CARule& r)
{
	if ( !PfwAppRuleInsert( (APP_RULE*) r.GetRule() ) )
		return FALSE;

	m_rules.insert( m_rules.begin(), r );
	return TRUE;
}
BOOL CAppRules::Delete(LPCTSTR appname)
{
	if ( m_rules.empty() ) return FALSE;

	list<CARule>::iterator iter = m_rules.begin();
	for ( ; iter != m_rules.end(); iter++ )
	{
		if ( _tcscmp(iter->GetRule()->app_name,appname) == 0 )
			break;
	}
	if ( iter == m_rules.end() )
		return FALSE;

	if ( !PfwAppRuleDelete( appname ) )
		return FALSE;

	m_rules.erase(iter);
	return TRUE;
}
BOOL CAppRules::Modify(const CARule& r)
{
	list<CARule>::iterator iter = m_rules.begin();
	for ( ; iter != m_rules.end(); iter++ )
	{
		if ( _tcscmp(iter->GetRule()->app_name,r.GetRule()->app_name) == 0 )
			break;
	}
	if ( iter == m_rules.end() )
		return FALSE;

	if ( !PfwAppRuleModify( (APP_RULE*)r.GetRule() ) )
		return FALSE;
    
	*iter = r;
	return TRUE;
}
const APP_RULE* CAppRules::GetRule( LPCTSTR appname ) const
{
	if ( m_rules.empty() ) return NULL;

	list<CARule>::const_iterator iter = m_rules.begin();
	for ( ; iter != m_rules.end(); iter++ )
	{
		if ( _tcscmp(iter->GetRule()->app_name,appname) == 0 )
			break;
	}
	if ( iter != m_rules.end() )
	{
		return iter->GetRule();
	}
	return NULL;
}
const APP_RULE* CAppRules::First()
{
	m_iter = m_rules.begin();
	return m_iter->GetRule();
}
const APP_RULE* CAppRules::Next()
{
	m_iter++;
	if ( m_iter == m_rules.end() )
	{
		return NULL;
	}
	else
	{
		return m_iter->GetRule();
	}
}

int CAppRules::LoadFromFile(LPCTSTR szFileName)
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
			DWORD rs;
			in.Read( &rs, sizeof(rs) );		//���ṹ��С
			APP_RULE *rule = (APP_RULE*) new BYTE[rs];
			in.Read( rule, rs );			//���ṹ����

			DWORD ss;
			in.Read( &ss, sizeof(ss) );		//����������С
			rule->app_name = (TCHAR*) new BYTE[ss];
			in.Read( rule->app_name, ss );	//��������

			if ( Insert(*rule) ) count++;	//����˹��򲢼���
		}
	}
	in.Close();
	return count;
}
int CAppRules::SaveToFile(LPCTSTR szFileName) const
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

	list<CARule>::const_iterator iter = m_rules.end();
	iter--;
	for ( DWORD i=0; i<cc; i++, iter-- )
	{
		const APP_RULE *rule = iter->GetRule();

		DWORD rs = rule->size;
		out.Write( &rs, sizeof(rs) );	//��ṹ��С
		out.Write( rule, rs );			//��ṹ����

		DWORD ss = sizeof(TCHAR) * ( _tcslen( rule->app_name ) + 1 );
		out.Write( &ss, sizeof(ss) );	//���������С
		out.Write( rule->app_name, ss );//�������
	}

	out.Close();
	return cc;
}

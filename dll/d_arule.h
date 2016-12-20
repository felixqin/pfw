#pragma once


class CARLNode;

/////////////////////////////////////////////////
class CAppRules
{
public:
	BOOL Insert( const APP_RULE &rule );
	BOOL Delete( LPCTSTR szAppName );
	BOOL Modify( const APP_RULE &rule );

	APP_RULE * Find( LPCTSTR szAppName );

public:
	CAppRules();
	~CAppRules();

private:
	CARLNode	*m_link;

private:
	CARLNode * find_prev( LPCTSTR szAppName );

};

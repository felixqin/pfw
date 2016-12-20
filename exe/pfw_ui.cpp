#include "stdafx.h"


void InsertListColumn( CListCtrl &ListCtrl, int nCol, UINT uHeadID, int nWidth)
{
	CString szHead;
	szHead.LoadString( uHeadID );
	ListCtrl.InsertColumn( nCol, szHead );
	ListCtrl.SetColumnWidth( nCol, nWidth );
}

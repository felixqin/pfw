#pragma once

#include "rule.h"
#include "log.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// PFW �ĳ�ʼ���ͷ���ʼ������
DLL_API BOOL PfwInitialize();
DLL_API void PfwUninitialize();

/*
�����ȫ�������ýӿں���

	˵��:
		FILTER_RULE �ṹ�е� name �� description ��ʹ��(�����ֶ�������)
*/
DLL_API BOOL PfwFilterRuleInsert( FILTER_RULE *pRule, long nPos );
DLL_API BOOL PfwFilterRuleDelete( long nPos );
DLL_API BOOL PfwFilterRuleModify( FILTER_RULE *pRule, long nPos );
DLL_API BOOL PfwFilterRuleMoveUp( long nPos );
DLL_API BOOL PfwFilterRuleMoveDown( long nPos );

/*
����Ӧ�ó���������ýӿں���

	˵��:
		APP_RULE �ṹ��ʹ�� app_name ��ʹ�� pid
*/
#ifdef UNICODE
DLL_API BOOL PfwAppRuleInsertW( APP_RULE *pRule );
DLL_API BOOL PfwAppRuleDeleteW( const wchar_t *szAppName );
DLL_API BOOL PfwAppRuleModifyW( APP_RULE *pRule );
#else
DLL_API BOOL PfwAppRuleInsertA( APP_RULE *pRule );
DLL_API BOOL PfwAppRuleDeleteA( const char *szAppName );
DLL_API BOOL PfwAppRuleModifyA( APP_RULE *pRule );
#endif

#ifdef UNICODE
#define PfwAppRuleInsert	PfwAppRuleInsertW
#define PfwAppRuleDelete	PfwAppRuleDeleteW
#define PfwAppRuleModify	PfwAppRuleModifyW
#else
#define PfwAppRuleInsert	PfwAppRuleInsertA
#define PfwAppRuleDelete	PfwAppRuleDeleteA
#define PfwAppRuleModify	PfwAppRuleModifyA
#endif


/*
��־����������ӿں���
*/
DLL_API BOOL PfwEnable( BOOL bEnable );
DLL_API HWND PfwSetNotifiedWnd( HWND hWnd );
DLL_API HWND PfwGetNotifiedWnd();


///////////////////////////////////////////////////////////////
//	��������
DLL_API UINT GetProcessFileName( HANDLE pid, LPTSTR lpFileName, UINT cchFileName );



#ifdef __cplusplus
}	//extern "C"
#endif

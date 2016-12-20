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

// PFW 的初始化和反初始化函数
DLL_API BOOL PfwInitialize();
DLL_API void PfwUninitialize();

/*
五个安全规则设置接口函数

	说明:
		FILTER_RULE 结构中的 name 和 description 不使用(即此字段无意义)
*/
DLL_API BOOL PfwFilterRuleInsert( FILTER_RULE *pRule, long nPos );
DLL_API BOOL PfwFilterRuleDelete( long nPos );
DLL_API BOOL PfwFilterRuleModify( FILTER_RULE *pRule, long nPos );
DLL_API BOOL PfwFilterRuleMoveUp( long nPos );
DLL_API BOOL PfwFilterRuleMoveDown( long nPos );

/*
三个应用程序规则设置接口函数

	说明:
		APP_RULE 结构中使用 app_name 不使用 pid
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
日志及驱动管理接口函数
*/
DLL_API BOOL PfwEnable( BOOL bEnable );
DLL_API HWND PfwSetNotifiedWnd( HWND hWnd );
DLL_API HWND PfwGetNotifiedWnd();


///////////////////////////////////////////////////////////////
//	其它函数
DLL_API UINT GetProcessFileName( HANDLE pid, LPTSTR lpFileName, UINT cchFileName );



#ifdef __cplusplus
}	//extern "C"
#endif

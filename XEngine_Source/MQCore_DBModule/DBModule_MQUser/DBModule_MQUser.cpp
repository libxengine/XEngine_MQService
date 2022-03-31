#include "pch.h"
#include "DBModule_MQUser.h"
/********************************************************************
//    Created:     2022/03/17  09:47:36
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQUser\DBModule_MQUser.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQUser
//    File Base:   DBModule_MQUser
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列用户信息操作
//    History:
*********************************************************************/
CDBModule_MQUser::CDBModule_MQUser()
{
    xhDBSQL = 0;
}
CDBModule_MQUser::~CDBModule_MQUser()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MQUser_Init
函数功能：初始化数据库管理器
 参数.一：pSt_DBConnector
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：数据MYSQL数据库连接信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector)
{
    DBModule_IsErrorOccur = FALSE;

    if (NULL == pSt_DBConnector)
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
        return FALSE;
    }
#ifdef _WINDOWS
    LPCTSTR lpszStrCharset = _T("gbk");
#else
    LPCTSTR lpszStrCharset = _T("utf8");
#endif
    //连接数据库
    _tcscpy(pSt_DBConnector->tszDBName, _T("XEngine_MQUser"));
    if (!DataBase_MySQL_Connect(&xhDBSQL, pSt_DBConnector, 5, TRUE, lpszStrCharset))
    {
        DBModule_IsErrorOccur = TRUE;
        DBModule_dwErrorCode = DataBase_GetLastError();
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_Destory()
{
    DBModule_IsErrorOccur = FALSE;
    DataBase_MySQL_Close(xhDBSQL);
    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_UserInsert
函数功能：插入用户
 参数.一：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
    DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[2048];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

    _stprintf(tszSQLStatement, _T("INSERT INTO `UserInfo` (tszUserName,tszUserPass,tszEMailAddr,nPhoneNumber,nIDNumber,nUserState,nUserLevel,tszCreateTime) VALUES('%s','%s','%s',%lld,%lld,%d,%d,now())"), pSt_UserInfo->tszUserName, pSt_UserInfo->tszUserPass, pSt_UserInfo->tszEMailAddr, pSt_UserInfo->nPhoneNumber, pSt_UserInfo->nIDNumber, pSt_UserInfo->nUserState, pSt_UserInfo->nUserLevel);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}

    return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_UserQuery
函数功能：查询用户
 参数.一：pSt_UserInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	//查询
	XHDATA xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf_s(tszSQLStatement, _T("SELECT * FROM `UserInfo` WHERE tszUserName = '%s' AND tszUserPass = '%s'"), pSt_UserInfo->tszUserName, pSt_UserInfo->tszUserPass);
	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return FALSE;
	}
	TCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
	if (NULL != pptszResult[3])
	{
		_tcscpy(pSt_UserInfo->tszEMailAddr, pptszResult[3]);
	}
	if (NULL != pptszResult[4])
	{
		pSt_UserInfo->nPhoneNumber = _ttoi64(pptszResult[4]);
	}
	if (NULL != pptszResult[5])
	{
		pSt_UserInfo->nIDNumber = _ttoi64(pptszResult[5]);
	}
	if (NULL != pptszResult[6])
	{
		pSt_UserInfo->nUserState = _ttoi(pptszResult[6]);
	}
	if (NULL != pptszResult[7])
	{
		pSt_UserInfo->nUserLevel = _ttoi(pptszResult[7]);
	}
	if (NULL != pptszResult[8])
	{
		_tcscpy(pSt_UserInfo->tszLoginTime, pptszResult[8]);
	}
	if (NULL != pptszResult[9])
	{
		_tcscpy(pSt_UserInfo->tszCreateTime, pptszResult[9]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_UserDelete
函数功能：删除用户
 参数.一：pSt_UserInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要删除的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf_s(tszSQLStatement, _T("DELETE FROM `UserInfo` WHERE tszUserName = '%s' AND tszUserPass = '%s'"), pSt_UserInfo->tszUserName, pSt_UserInfo->tszUserPass);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_UserUPDate
函数功能：更新用户
 参数.一：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要处理的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserInfo)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	if (1 == pSt_UserInfo->nUserState)
	{
		_stprintf_s(tszSQLStatement, _T("UPDATE `UserInfo` SET nUserState = %d,tszLoginTime = now() WHERE tszUserName = '%s'"), pSt_UserInfo->nUserState, pSt_UserInfo->tszUserName);
	}
	else
	{
		_stprintf_s(tszSQLStatement, _T("UPDATE `UserInfo` SET nUserState = %d WHERE tszUserName = '%s'"), pSt_UserInfo->nUserState, pSt_UserInfo->tszUserName);
	}
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DBModule_MQUser_KeyInsert
函数功能：插入一个绑定的用户消息
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的内容
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[2048];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf(tszSQLStatement, _T("INSERT INTO `UserKey` (tszKeyUser,tszKeyName,nKeySerial,tszUPTime,tszCreateTime) VALUES('%s','%s',%lld,now(),now())"), pSt_UserKey->tszKeyName, pSt_UserKey->tszKeyName, pSt_UserKey->nKeySerial);

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}

	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyQuery
函数功能：通过用户查询绑定的消息信息
 参数.一：pSt_UserKey
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息,输出查询到的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	//查询
	XHDATA xhTable = 0;
	__int64u nllLine = 0;
	__int64u nllRow = 0;

	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));
	//名称为,消息名为必填
	_stprintf_s(tszSQLStatement, _T("SELECT * FROM `UserKey` WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName);
	if (!DataBase_MySQL_ExecuteQuery(xhDBSQL, &xhTable, tszSQLStatement, &nllLine, &nllRow))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	if (nllLine <= 0)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_EMPTY;
		return FALSE;
	}
	TCHAR** pptszResult = DataBase_MySQL_GetResult(xhDBSQL, xhTable);
	if (NULL != pptszResult[2])
	{
		_tcscpy(pSt_UserKey->tszKeyName, pptszResult[2]);
	}
	if (NULL != pptszResult[3])
	{
		pSt_UserKey->nKeySerial = _ttoi64(pptszResult[3]);
	}
	if (NULL != pptszResult[4])
	{
		_tcscpy(pSt_UserKey->tszUPTime, pptszResult[4]);
	}
	if (NULL != pptszResult[5])
	{
		_tcscpy(pSt_UserKey->tszCreateTime, pptszResult[5]);
	}
	DataBase_MySQL_FreeResult(xhDBSQL, xhTable);
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyDelete
函数功能：删除绑定的消息队列
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要删除的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	if (_tcslen(pSt_UserKey->tszKeyName) <= 0)
	{
		_stprintf_s(tszSQLStatement, _T("DELETE FROM `UserKey` WHERE tszKeyUser = '%s'"), pSt_UserKey->tszUserName);
	}
	else
	{
		_stprintf_s(tszSQLStatement, _T("DELETE FROM `UserKey` WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName);
	}

	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}
	return TRUE;
}
/********************************************************************
函数名称：DBModule_MQUser_KeyUPDate
函数功能：更新用户绑定的消息
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要更新的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDBModule_MQUser::DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY* pSt_UserKey)
{
	DBModule_IsErrorOccur = FALSE;

	if (NULL == pSt_UserKey)
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = ERROR_XENGINE_MQCORE_DATABASE_PARAMENT;
		return FALSE;
	}
	TCHAR tszSQLStatement[1024];
	memset(tszSQLStatement, '\0', sizeof(tszSQLStatement));

	_stprintf_s(tszSQLStatement, _T("UPDATE `UserKey` SET nKeySerial = %lld WHERE tszKeyUser = '%s' AND tszKeyName = '%s'"), pSt_UserKey->nKeySerial, pSt_UserKey->tszUserName, pSt_UserKey->tszKeyName);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}

	return TRUE;
}
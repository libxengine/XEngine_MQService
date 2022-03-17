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
BOOL CDBModule_MQUser::DBModule_MQUser_UserInsert(XENGINE_DBUSERINFO* pSt_UserInfo)
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

    _stprintf(tszSQLStatement, _T("INSERT INTO `UserInfo` (tszUserName,tszUserPass,tszEMailAddr,nPhoneNumber,nIDNumber,nMQKey,nUserState,nUserLevel,tszLoginTime,tszCreateTime) VALUES('%s','%s','%s',%lld,%lld,%d,%d,%d,'%s',now())"), pSt_UserInfo->st_UserInfo.tszUserName, pSt_UserInfo->st_UserInfo.tszUserPass, pSt_UserInfo->st_UserInfo.tszEMailAddr, pSt_UserInfo->st_UserInfo.nPhoneNumber, pSt_UserInfo->st_UserInfo.nIDNumber, pSt_UserInfo->nMQKey, pSt_UserInfo->st_UserInfo.nUserState, pSt_UserInfo->st_UserInfo.nUserLevel, pSt_UserInfo->st_UserInfo.tszLoginTime);
	if (!DataBase_MySQL_Execute(xhDBSQL, tszSQLStatement))
	{
		DBModule_IsErrorOccur = TRUE;
		DBModule_dwErrorCode = DataBase_GetLastError();
		return FALSE;
	}

    return TRUE;
}
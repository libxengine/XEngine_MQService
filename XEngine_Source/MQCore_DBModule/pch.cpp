#include "pch.h"
#include "DBModule_MQData/DBModule_MQData.h"
#include "DBModule_MQUser/DBModule_MQUser.h"
/********************************************************************
//    Created:     2022/03/15  16:00:31
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出实现
//    History:
*********************************************************************/
BOOL DBModule_IsErrorOccur = FALSE;
DWORD DBModule_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CDBModule_MQData m_DBData;
CDBModule_MQUser m_DBUser;
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD DBModule_GetLastError(int* pInt_SysError)
{
	if (NULL != pInt_SysError)
	{
		*pInt_SysError = errno;
	}
	return DBModule_dwErrorCode;
}
/*************************************************************************
						消息队列导出函数
**************************************************************************/
extern "C" BOOL DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO * pSt_DBConnector, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, LPVOID lParam)
{
	return m_DBData.DBModule_MQData_Init(pSt_DBConnector, fpCall_TimePublish, lParam);
}
extern "C" BOOL DBModule_MQData_Destory()
{
	return m_DBData.DBModule_MQData_Destory();
}
extern "C" BOOL DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE * pSt_DBManage)
{
	return m_DBData.DBModule_MQData_Insert(pSt_DBManage);
}
extern "C" BOOL DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE * pSt_DBInfo)
{
	return m_DBData.DBModule_MQData_Query(pSt_DBInfo);
}
extern "C" BOOL DBModule_MQData_GetSerial(LPCTSTR lpszName, __int64x * pInt_DBCount, XENGINE_DBMESSAGEQUEUE * pSt_DBStart, XENGINE_DBMESSAGEQUEUE * pSt_DBEnd)
{
	return m_DBData.DBModule_MQData_GetSerial(lpszName, pInt_DBCount, pSt_DBStart, pSt_DBEnd);
}
extern "C" BOOL DBModule_MQData_CreateTable(LPCTSTR lpszQueueName)
{
	return m_DBData.DBModule_MQData_CreateTable(lpszQueueName);
}
extern "C" BOOL DBModule_MQData_DeleteTable(LPCTSTR lpszQueueName)
{
	return m_DBData.DBModule_MQData_DeleteTable(lpszQueueName);
}
extern "C" BOOL DBModule_MQData_TimeInsert(XENGINE_DBTIMERELEASE * pSt_DBInfo)
{
	return m_DBData.DBModule_MQData_TimeInsert(pSt_DBInfo);
}
extern "C" BOOL DBModule_MQData_TimeQuery(XENGINE_DBTIMERELEASE * **pppSt_DBInfo, int* pInt_ListCount)
{
	return m_DBData.DBModule_MQData_TimeQuery(pppSt_DBInfo, pInt_ListCount);
}
extern "C" BOOL DBModule_MQData_TimeDelete(XENGINE_DBTIMERELEASE * pSt_DBInfo)
{
	return m_DBData.DBModule_MQData_TimeDelete(pSt_DBInfo);
}
/*************************************************************************
						消息用户导出函数
**************************************************************************/
extern "C" BOOL DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO * pSt_DBConnector)
{
	return m_DBUser.DBModule_MQUser_Init(pSt_DBConnector);
}
extern "C" BOOL DBModule_MQUser_Destory()
{
	return m_DBUser.DBModule_MQUser_Destory();
}
extern "C" BOOL DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserInsert(pSt_UserInfo);
}
extern "C" BOOL DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserQuery(pSt_UserInfo);
}
extern "C" BOOL DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserDelete(pSt_UserInfo);
}
extern "C" BOOL DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserUPDate(pSt_UserInfo);
}
/*************************************************************************
						消息绑定导出函数
**************************************************************************/
extern "C" BOOL DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyInsert(pSt_UserKey);
}
extern "C" BOOL DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyQuery(pSt_UserKey);
}
extern "C" BOOL DBModule_MQUser_KeyList(LPCTSTR lpszUser, XENGINE_DBUSERKEY * **pppSt_UserKey, int* pInt_ListCount)
{
	return m_DBUser.DBModule_MQUser_KeyList(lpszUser, pppSt_UserKey, pInt_ListCount);
}
extern "C" BOOL DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyDelete(pSt_UserKey);
}
extern "C" BOOL DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyUPDate(pSt_UserKey);
}
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
bool DBModule_IsErrorOccur = false;
XLONG DBModule_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CDBModule_MQData m_DBData;
CDBModule_MQUser m_DBUser;
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG DBModule_GetLastError(int* pInt_SysError)
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
extern "C" bool DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO * pSt_DBConnector, bool bMemoryQuery, bool bMemoryInsert)
{
	return m_DBData.DBModule_MQData_Init(pSt_DBConnector, bMemoryQuery, bMemoryInsert);
}
extern "C" bool DBModule_MQData_Destory()
{
	return m_DBData.DBModule_MQData_Destory();
}
extern "C" bool DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE * pSt_DBManage)
{
	return m_DBData.DBModule_MQData_Insert(pSt_DBManage);
}
extern "C" bool DBModule_MQData_Delete(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo)
{
	return m_DBData.DBModule_MQData_Delete(pSt_DBInfo);
}
extern "C" bool DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE * pSt_DBInfo)
{
	return m_DBData.DBModule_MQData_Query(pSt_DBInfo);
}
extern "C" bool DBModule_MQData_Modify(XENGINE_DBMESSAGEQUEUE * pSt_DBInfo)
{
	return m_DBData.DBModule_MQData_Modify(pSt_DBInfo);
}
extern "C" bool DBModule_MQData_GetSerial(LPCXSTR lpszName, __int64x * pInt_DBCount, XENGINE_DBMESSAGEQUEUE * pSt_DBStart, XENGINE_DBMESSAGEQUEUE * pSt_DBEnd)
{
	return m_DBData.DBModule_MQData_GetSerial(lpszName, pInt_DBCount, pSt_DBStart, pSt_DBEnd);
}
extern "C" bool DBModule_MQData_CreateTable(LPCXSTR lpszQueueName)
{
	return m_DBData.DBModule_MQData_CreateTable(lpszQueueName);
}
extern "C" bool DBModule_MQData_DeleteTable(LPCXSTR lpszQueueName)
{
	return m_DBData.DBModule_MQData_DeleteTable(lpszQueueName);
}
extern "C" bool DBModule_MQData_ModifyTable(LPCXSTR lpszSrcTable, LPCXSTR lpszDstTable)
{
	return m_DBData.DBModule_MQData_ModifyTable(lpszSrcTable, lpszDstTable);
}
extern "C" bool DBModule_MQData_ShowTable(XCHAR * **pppszTableName, int* pInt_ListCount)
{
	return m_DBData.DBModule_MQData_ShowTable(pppszTableName, pInt_ListCount);
}
extern "C" bool DBModule_MQData_GetLeftCount(LPCXSTR lpszTableName, __int64x nSerial, int* pInt_Count)
{
	return m_DBData.DBModule_MQData_GetLeftCount(lpszTableName, nSerial, pInt_Count);
}
/*************************************************************************
						消息用户导出函数
**************************************************************************/
extern "C" bool DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO * pSt_DBConnector, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, XPVOID lParam)
{
	return m_DBUser.DBModule_MQUser_Init(pSt_DBConnector, fpCall_TimePublish, lParam);
}
extern "C" bool DBModule_MQUser_Destory()
{
	return m_DBUser.DBModule_MQUser_Destory();
}
extern "C" bool DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserInsert(pSt_UserInfo);
}
extern "C" bool DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserQuery(pSt_UserInfo);
}
extern "C" bool DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserDelete(pSt_UserInfo);
}
extern "C" bool DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_DBUser.DBModule_MQUser_UserUPDate(pSt_UserInfo);
}
extern "C" bool DBModule_MQUser_UserList(XENGINE_PROTOCOL_USERINFO * **pppSt_UserInfo, int* pInt_ListCount)
{
	return m_DBUser.DBModule_MQUser_UserList(pppSt_UserInfo, pInt_ListCount);
}
/*************************************************************************
						消息绑定导出函数
**************************************************************************/
extern "C" bool DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyInsert(pSt_UserKey);
}
extern "C" bool DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyQuery(pSt_UserKey);
}
extern "C" bool DBModule_MQUser_KeyList(LPCXSTR lpszUser, LPCXSTR lpszKeyName, XENGINE_DBUSERKEY * **pppSt_UserKey, int* pInt_ListCount)
{
	return m_DBUser.DBModule_MQUser_KeyList(lpszUser, lpszKeyName, pppSt_UserKey, pInt_ListCount);
}
extern "C" bool DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyDelete(pSt_UserKey);
}
extern "C" bool DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY * pSt_UserKey)
{
	return m_DBUser.DBModule_MQUser_KeyUPDate(pSt_UserKey);
}
extern "C" bool DBModule_MQUser_KeyTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable)
{
	return m_DBUser.DBModule_MQUser_KeyTopicUPDate(lpszSourceTable, lpszDestTable);
}
/*************************************************************************
						定时发布导出函数
**************************************************************************/
extern "C" bool DBModule_MQUser_TimeInsert(XENGINE_DBTIMERELEASE * pSt_DBInfo)
{
	return m_DBUser.DBModule_MQUser_TimeInsert(pSt_DBInfo);
}
extern "C" bool DBModule_MQUser_TimeQuery(XENGINE_DBTIMERELEASE * **pppSt_DBInfo, int* pInt_ListCount)
{
	return m_DBUser.DBModule_MQUser_TimeQuery(pppSt_DBInfo, pInt_ListCount);
}
extern "C" bool DBModule_MQUser_TimeDelete(XENGINE_DBTIMERELEASE * pSt_DBInfo)
{
	return m_DBUser.DBModule_MQUser_TimeDelete(pSt_DBInfo);
}
extern "C" bool DBModule_MQUser_TimeUPDate(XENGINE_DBTIMERELEASE * pSt_DBInfo)
{
	return m_DBUser.DBModule_MQUser_TimeUPDate(pSt_DBInfo);
}
extern "C" bool DBModule_MQUser_TimeClaer(time_t nTime)
{
	return m_DBUser.DBModule_MQUser_TimeClaer(nTime);
}
extern "C" bool DBModule_MQUser_TimeTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable)
{
	return m_DBUser.DBModule_MQUser_TimeTopicUPDate(lpszSourceTable, lpszDestTable);
}
/*************************************************************************
						主题所有者导出函数
**************************************************************************/
extern "C" bool DBModule_MQUser_OwnerInsert(XENGINE_DBTOPICOWNER * pSt_DBOwner)
{
	return m_DBUser.DBModule_MQUser_OwnerInsert(pSt_DBOwner);
}
extern "C" bool DBModule_MQUser_OwnerDelete(XENGINE_DBTOPICOWNER * pSt_DBOwner)
{
	return m_DBUser.DBModule_MQUser_OwnerDelete(pSt_DBOwner);
}
extern "C" bool DBModule_MQUser_OwnerQuery(XENGINE_DBTOPICOWNER * pSt_DBOwner)
{
	return m_DBUser.DBModule_MQUser_OwnerQuery(pSt_DBOwner);
}
extern "C" bool DBModule_MQUser_OwnerTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable)
{
	return m_DBUser.DBModule_MQUser_OwnerTopicUPDate(lpszSourceTable, lpszDestTable);
}
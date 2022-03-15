#include "pch.h"
#include "DBModule_DDSMessage/DBModule_DDSMessage.h"
#include "DBModule_MessageQueue/DBModule_MessageQueue.h"
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
CDBModule_DDSMessage m_DDService;
CDBModule_MessageQueue m_MessageQueue;
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
extern "C" BOOL DBModule_MessageQueue_Init(DATABASE_MYSQL_CONNECTINFO * pSt_DBConnector)
{
	return m_MessageQueue.DBModule_MessageQueue_Init(pSt_DBConnector);
}
extern "C" BOOL DBModule_MessageQueue_Destory()
{
	return m_MessageQueue.DBModule_MessageQueue_Destory();
}
extern "C" BOOL DBModule_MessageQueue_Insert(XENGINE_DBMESSAGEQUEUE * pSt_DBManage)
{
	return m_MessageQueue.DBModule_MessageQueue_Insert(pSt_DBManage);
}
extern "C" BOOL DBModule_MessageQueue_CreateTable(LPCTSTR lpszQueueName)
{
	return m_MessageQueue.DBModule_MessageQueue_CreateTable(lpszQueueName);
}
/*************************************************************************
						消息分发导出函数
**************************************************************************/
extern "C" BOOL DBModule_DDSMessage_Init(DATABASE_MYSQL_CONNECTINFO * pSt_DBConnector)
{
	return m_DDService.DBModule_DDSMessage_Init(pSt_DBConnector);
}
extern "C" BOOL DBModule_DDSMessage_Destory()
{
	return m_DDService.DBModule_DDSMessage_Destory();
}
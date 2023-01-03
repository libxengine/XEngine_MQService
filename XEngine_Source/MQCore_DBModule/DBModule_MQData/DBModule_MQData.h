#pragma once
/********************************************************************
//    Created:     2022/03/17  09:46:20
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQData\DBModule_MQData.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQData
//    File Base:   DBModule_MQData
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列数据库操作
//    History:
*********************************************************************/

class CDBModule_MQData
{
public:
	CDBModule_MQData();
	~CDBModule_MQData();
public:
	BOOL DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
	BOOL DBModule_MQData_Destory();
	BOOL DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
	BOOL DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
	BOOL DBModule_MQData_Modify(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
	BOOL DBModule_MQData_GetSerial(LPCTSTR lpszName, __int64x* pInt_DBCount, XENGINE_DBMESSAGEQUEUE* pSt_DBStart, XENGINE_DBMESSAGEQUEUE* pSt_DBEnd);
	BOOL DBModule_MQData_CreateTable(LPCTSTR lpszQueueName);
	BOOL DBModule_MQData_DeleteTable(LPCTSTR lpszQueueName);
	BOOL DBModule_MQData_ModifyTable(LPCTSTR lpszSrcTable, LPCTSTR lpszDstTable);
private:
	XHDATA xhDBSQL;
};
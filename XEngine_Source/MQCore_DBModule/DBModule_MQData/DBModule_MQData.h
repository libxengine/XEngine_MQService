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
	bool DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
	bool DBModule_MQData_Destory();
	bool DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
	bool DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
	bool DBModule_MQData_Modify(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
	bool DBModule_MQData_List(LPCXSTR lpszQueueName, __int64x nSerial, XENGINE_DBMESSAGEQUEUE*** pppSt_DBMessage, int* pInt_ListCount);
	bool DBModule_MQData_GetSerial(LPCXSTR lpszName, __int64x* pInt_DBCount, XENGINE_DBMESSAGEQUEUE* pSt_DBStart, XENGINE_DBMESSAGEQUEUE* pSt_DBEnd);
	bool DBModule_MQData_CreateTable(LPCXSTR lpszQueueName);
	bool DBModule_MQData_DeleteTable(LPCXSTR lpszQueueName);
	bool DBModule_MQData_ModifyTable(LPCXSTR lpszSrcTable, LPCXSTR lpszDstTable);
	bool DBModule_MQData_ShowTable(XCHAR*** pppszTableName, int* pInt_ListCount);
	bool DBModule_MQData_GetLeftCount(LPCXSTR lpszTableName, int nSerial, int* pInt_Count);
private:
	XNETHANDLE xhDBSQL;
};
#pragma once
/********************************************************************
//    Created:     2022/03/15  13:08:03
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MessageQueue\DBModule_MessageQueue.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MessageQueue
//    File Base:   DBModule_MessageQueue
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列数据库操作
//    History:
*********************************************************************/

class CDBModule_MessageQueue
{
public:
	CDBModule_MessageQueue();
	~CDBModule_MessageQueue();
public:
	BOOL DBModule_MessageQueue_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
	BOOL DBModule_MessageQueue_Destory();
	BOOL DBModule_MessageQueue_Insert(XENGINE_DBMESSAGEQUEUE* pSt_DBManage);
	BOOL DBModule_MessageQueue_CreateTable(LPCTSTR lpszQueueName);
private:
	XHDATA xhDBSQL;
};
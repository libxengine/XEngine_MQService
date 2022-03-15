#pragma once
/********************************************************************
//    Created:     2022/02/23  16:08:19
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_DDSMessage\DBModule_DDSMessage.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_DDSMessage
//    File Base:   DBModule_DDSMessage
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     DDS数据库操作
//    History:
*********************************************************************/

class CDBModule_DDSMessage
{
public:
	CDBModule_DDSMessage();
	~CDBModule_DDSMessage();
public:
	BOOL DBModule_DDSMessage_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
	BOOL DBModule_DDSMessage_Destory();
private:
	XHDATA xhDBSQL;
};
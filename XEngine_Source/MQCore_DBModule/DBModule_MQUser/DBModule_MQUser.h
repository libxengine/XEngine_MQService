#pragma once
/********************************************************************
//    Created:     2022/03/17  09:47:28
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQUser\DBModule_MQUser.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_MQUser
//    File Base:   DBModule_MQUser
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列用户信息操作
//    History:
*********************************************************************/

class CDBModule_MQUser
{
public:
	CDBModule_MQUser();
	~CDBModule_MQUser();
public:
	BOOL DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
	BOOL DBModule_MQUser_Destory();
private:
	XHDATA xhDBSQL;
};
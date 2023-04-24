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
	bool DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, XPVOID lParam = NULL);
	bool DBModule_MQUser_Destory();
	bool DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
	bool DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
	bool DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
	bool DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
public:
	bool DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY* pSt_UserKey);
	bool DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY* pSt_UserKey);
	bool DBModule_MQUser_KeyList(LPCXSTR lpszUser, LPCXSTR lpszKeyName, XENGINE_DBUSERKEY*** pppSt_UserKey, int* pInt_ListCount);
	bool DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY* pSt_UserKey);
	bool DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY* pSt_UserKey);
	bool DBModule_MQUser_KeyTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable);
public:
	bool DBModule_MQUser_TimeInsert(XENGINE_DBTIMERELEASE* pSt_DBInfo);
	bool DBModule_MQUser_TimeQuery(XENGINE_DBTIMERELEASE*** pppSt_DBInfo, int* pInt_ListCount);
	bool DBModule_MQUser_TimeDelete(XENGINE_DBTIMERELEASE* pSt_DBInfo);
	bool DBModule_MQUser_TimeUPDate(XENGINE_DBTIMERELEASE* pSt_DBInfo);
	bool DBModule_MQUser_TimeClaer(time_t nTime = 0);
	bool DBModule_MQUser_TimeTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable);
public:
	bool DBModule_MQUser_OwnerInsert(XENGINE_DBTOPICOWNER* pSt_DBOwner);
	bool DBModule_MQUser_OwnerDelete(XENGINE_DBTOPICOWNER* pSt_DBOwner);
	bool DBModule_MQUser_OwnerQuery(XENGINE_DBTOPICOWNER* pSt_DBOwner);
	bool DBModule_MQUser_OwnerTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable);
protected:
	static XHTHREAD CALLBACK DBModule_MQUser_TimeThread(XPVOID lParam);
private:
	shared_ptr<std::thread> pSTDThread;
private:
	XPVOID m_lParam;
	CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH lpCall_TimePublish;
private:
	bool bIsRun;
	XNETHANDLE xhDBSQL;
};
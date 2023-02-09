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
	BOOL DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, LPVOID lParam = NULL);
	BOOL DBModule_MQUser_Destory();
	BOOL DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
	BOOL DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
	BOOL DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
	BOOL DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
public:
	BOOL DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY* pSt_UserKey);
	BOOL DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY* pSt_UserKey);
	BOOL DBModule_MQUser_KeyList(LPCTSTR lpszUser, LPCTSTR lpszKeyName, XENGINE_DBUSERKEY*** pppSt_UserKey, int* pInt_ListCount);
	BOOL DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY* pSt_UserKey);
	BOOL DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY* pSt_UserKey);
public:
	BOOL DBModule_MQUser_TimeInsert(XENGINE_DBTIMERELEASE* pSt_DBInfo);
	BOOL DBModule_MQUser_TimeQuery(XENGINE_DBTIMERELEASE*** pppSt_DBInfo, int* pInt_ListCount);
	BOOL DBModule_MQUser_TimeDelete(XENGINE_DBTIMERELEASE* pSt_DBInfo);
	BOOL DBModule_MQUser_TimeClaer(time_t nTime = 0);
public:
	BOOL DBModule_MQUser_OwnerInsert(XENGINE_DBTOPICOWNER* pSt_DBOwner);
	BOOL DBModule_MQUser_OwnerDelete(XENGINE_DBTOPICOWNER* pSt_DBOwner);
	BOOL DBModule_MQUser_OwnerQuery(XENGINE_DBTOPICOWNER* pSt_DBOwner);
protected:
	static XHTHREAD CALLBACK DBModule_MQUser_TimeThread(LPVOID lParam);
private:
	shared_ptr<std::thread> pSTDThread;
private:
	LPVOID m_lParam;
	CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH lpCall_TimePublish;
private:
	BOOL bIsRun;
	XNETHANDLE xhDBSQL;
};
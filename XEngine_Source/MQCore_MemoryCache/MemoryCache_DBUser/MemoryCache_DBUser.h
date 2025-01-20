#pragma once
/********************************************************************
//    Created:     2025/01/17  10:08:29
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBUser\MemoryCache_DBUser.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBUser
//    File Base:   MemoryCache_DBUser
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     数据库高速缓存
//    History:
*********************************************************************/
typedef struct  
{
	XENGINE_PROTOCOL_USERINFO st_DBUserInfo;
	time_t nTimeStart;
	time_t nTimeLast;
}MEMORYCACHE_DBUSER;

class CMemoryCache_DBUser
{
public:
	CMemoryCache_DBUser();
	~CMemoryCache_DBUser();
public:
	bool MemoryCache_DBUser_Init(int nTimeLast, int nTimeCount, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE fpCall_MemoryCache, XPVOID lParam = NULL);
	bool MemoryCache_DBUser_SetHandle(XNETHANDLE xhDBSQL);
	bool MemoryCache_DBUser_Destory();
public:
	bool MemoryCache_DBUser_DataInsert(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo);
	bool MemoryCache_DBUser_DataQuery(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo);
	bool MemoryCache_DBUser_DataDelete(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo);
protected:
	static XHTHREAD CALLBACK DBModule_MQUser_TimeThread(XPVOID lParam);
private:
	bool bIsRun = false;
	int m_nTimeLast = 0;
	int m_nTimeCount = 0;
	XNETHANDLE m_xhDBSQL = 0;

	XPVOID m_lParam;
	CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE lpCall_MemoryCache;
private:
	std::shared_mutex st_LockerQuery;
	std::unique_ptr<std::thread> pSTDThread_Query;
private:
	std::unordered_map<xstring, MEMORYCACHE_DBUSER> stl_MapQuery;
};
#pragma once
/********************************************************************
//    Created:     2024/11/21  16:35:19
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBData\MemoryCache_DBData.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBData
//    File Base:   MemoryCache_DBData
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     数据库高速缓存
//    History:
*********************************************************************/
typedef struct  
{
	XENGINE_DBMESSAGEQUEUE st_DBMessageInfo;
	time_t nTimeStart;
	time_t nTimeLast;
}MEMORYCACHE_DBINFO;
typedef struct
{
	XENGINE_DBMESSAGEQUEUE st_DBMessageInfo;
	std::string m_StrSQL;
}MEMORYCACHE_DBINSERT;

struct TupleHash 
{
	template <typename... Args>
	std::size_t operator()(const std::tuple<Args...>& t) const 
	{
		return std::apply([](auto&&... args)
			{
			std::size_t seed = 0;
			((seed ^= std::hash<std::decay_t<decltype(args)>>()(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)), ...);
			return seed;
			}, t);
	}
};

class CMemoryCache_DBData
{
public:
	CMemoryCache_DBData();
	~CMemoryCache_DBData();
public:
	bool MemoryCache_DBData_Init(int nTimeLast, int nTimeCount, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE fpCall_MemoryCache, XPVOID lParam = NULL);
	bool MemoryCache_DBData_SetHandle(XNETHANDLE xhDBSQL);
	bool MemoryCache_DBData_Destory();
public:
	bool MemoryCache_DBData_DataInsert(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
	bool MemoryCache_DBData_DataQuery(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
	bool MemoryCache_DBData_DataDelete(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
public:
	bool MemoryCache_DBData_QueueInsert(LPCXSTR lpszSQLStr, XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
protected:
	static XHTHREAD CALLBACK DBModule_MQUser_TimeThread(XPVOID lParam);
	static XHTHREAD CALLBACK DBModule_MQUser_InsertThread(XPVOID lParam);
private:
	bool bIsRun = false;
	int m_nTimeLast = 0;
	int m_nTimeCount = 0;
	XNETHANDLE m_xhDBSQL = 0;

	XPVOID m_lParam;
	CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE lpCall_MemoryCache;
private:
	std::shared_mutex st_LockerList;
	std::shared_mutex st_LockerQuery;
	std::unique_ptr<std::thread> pSTDThread_Query;
	std::unique_ptr<std::thread> pSTDThread_Insert;
private:
	std::list<MEMORYCACHE_DBINSERT> stl_ListInsert;
	std::unordered_map<std::string, std::atomic<__int64x>> stl_MapSerial;
	std::unordered_map<std::tuple<__int64x, std::string>, MEMORYCACHE_DBINFO, TupleHash> stl_MapQuery;
};
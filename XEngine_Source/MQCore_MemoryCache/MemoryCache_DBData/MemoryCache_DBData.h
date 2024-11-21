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

struct TupleHash 
{
	template <typename... Args>
	std::size_t operator()(const std::tuple<Args...>& t) const 
	{
		return std::apply([](auto&&... args)
			{
			// 使用std::hash对每个元素分别哈希并组合结果
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
	bool MemoryCache_DBData_Init(int nTimeLast = 3600, int nTimeStart = 0);
	bool MemoryCache_DBData_Destory();
public:
	bool MemoryCache_DBData_DataInsert(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
	bool MemoryCache_DBData_DataQuery(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
	bool MemoryCache_DBData_DataDelete(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
protected:
	static XHTHREAD CALLBACK DBModule_MQUser_TimeThread(XPVOID lParam);
private:
	bool bIsRun = false;
	int m_nTimeLast = 0;
	int m_nTimeStart = 0;
private:
	std::shared_mutex st_LockerList;
	std::shared_mutex st_LockerQuery;
	std::unique_ptr<std::thread> pSTDThread;
private:
	std::list<std::string> stl_ListInsert;
	std::unordered_map<std::tuple<__int64x, std::string>, MEMORYCACHE_DBINFO, TupleHash> stl_MapQuery;
};
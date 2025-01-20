#include "pch.h"
#include "MemoryCache_DBData/MemoryCache_DBData.h"
#include "MemoryCache_DBUser/MemoryCache_DBUser.h"
/********************************************************************
//    Created:     2024/11/21  15:01:53
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出实现
//    History:
*********************************************************************/
bool MemoryCache_IsErrorOccur = false;
XLONG MemoryCache_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CMemoryCache_DBData m_MemoryDBData;
CMemoryCache_DBUser m_MemoryDBUser;
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG MemoryCache_GetLastError(int* pInt_SysError)
{
    if (NULL != pInt_SysError)
    {
        *pInt_SysError = errno;
    }
    return MemoryCache_dwErrorCode;
}
/*************************************************************************
                        高速缓存导出函数
**************************************************************************/
extern "C" bool MemoryCache_DBData_Init(int nTimeLast, int nTimeCount, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE fpCall_MemoryCache, XPVOID lParam)
{
    return m_MemoryDBData.MemoryCache_DBData_Init(nTimeLast, nTimeCount, fpCall_MemoryCache, lParam);
}
extern "C" bool MemoryCache_DBData_SetHandle(XNETHANDLE xhDBSQL)
{
    return m_MemoryDBData.MemoryCache_DBData_SetHandle(xhDBSQL);
}
extern "C" bool MemoryCache_DBData_Destory()
{
    return m_MemoryDBData.MemoryCache_DBData_Destory();
}
extern "C" bool MemoryCache_DBData_DataInsert(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
    return m_MemoryDBData.MemoryCache_DBData_DataInsert(pSt_DBMessageInfo);
}
extern "C" bool MemoryCache_DBData_DataQuery(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
    return m_MemoryDBData.MemoryCache_DBData_DataQuery(pSt_DBMessageInfo);
}
extern "C" bool MemoryCache_DBData_DataDelete(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
    return m_MemoryDBData.MemoryCache_DBData_DataDelete(pSt_DBMessageInfo);
}
extern "C" bool MemoryCache_DBData_QueueInsert(LPCXSTR lpszSQLStr, XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
    return m_MemoryDBData.MemoryCache_DBData_QueueInsert(lpszSQLStr, pSt_DBMessageInfo);
}
/*************************************************************************
                        用户信息高速缓存导出函数
**************************************************************************/
extern "C" bool MemoryCache_DBUser_Init(int nTimeLast, int nTimeCount, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE fpCall_MemoryCache, XPVOID lParam)
{
    return m_MemoryDBUser.MemoryCache_DBUser_Init(nTimeLast, nTimeCount, fpCall_MemoryCache, lParam);
}
extern "C" bool MemoryCache_DBUser_SetHandle(XNETHANDLE xhDBSQL)
{
    return m_MemoryDBUser.MemoryCache_DBUser_SetHandle(xhDBSQL);
}
extern "C" bool MemoryCache_DBUser_Destory()
{
    return m_MemoryDBUser.MemoryCache_DBUser_Destory();
}
extern "C" bool MemoryCache_DBUser_DataInsert(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo)
{
    return m_MemoryDBUser.MemoryCache_DBUser_DataInsert(pSt_DBUserInfo);
}
extern "C" bool MemoryCache_DBUser_DataQuery(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo)
{
    return m_MemoryDBUser.MemoryCache_DBUser_DataQuery(pSt_DBUserInfo);
}
extern "C" bool MemoryCache_DBUser_DataDelete(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo)
{
    return m_MemoryDBUser.MemoryCache_DBUser_DataDelete(pSt_DBUserInfo);
}
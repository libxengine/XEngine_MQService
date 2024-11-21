#include "pch.h"
#include "MemoryCache_DBData/MemoryCache_DBData.h"
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
extern "C" bool MemoryCache_DBData_Init(int nTimeLast, int nTimeStart)
{
    return m_MemoryDBData.MemoryCache_DBData_Init(nTimeLast, nTimeStart);
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
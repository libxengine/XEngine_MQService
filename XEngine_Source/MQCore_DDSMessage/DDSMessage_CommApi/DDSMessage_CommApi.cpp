#include "pch.h"
#include "DDSMessage_CommApi.h"
/********************************************************************
//    Created:     2021/09/22  13:18:04
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\DDSMessage_CommApi\DDSMessage_CommApi.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DDSMessage\DDSMessage_CommApi
//    File Base:   DDSMessage_CommApi
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     服务器类公有API类
//    History:
*********************************************************************/
CDDSMessage_CommApi::CDDSMessage_CommApi()
{
}
CDDSMessage_CommApi::~CDDSMessage_CommApi()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：DDSMessage_CommApi_Init
函数功能：初始化DDS服务
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDDSMessage_CommApi::DDSMessage_CommApi_Init()
{
    XDDS_IsErrorOccur = FALSE;

    return TRUE;
}
/************************************************************************
函数名称：DDSMessage_CommApi_Destory
函数功能：销毁DDS服务
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
BOOL CDDSMessage_CommApi::DDSMessage_CommApi_Destory()
{
    XDDS_IsErrorOccur = FALSE;

    st_Locker.lock();
    stl_MapDDSClient.clear();
    st_Locker.unlock();

    return TRUE;
}
/********************************************************************
函数名称：DDSMessage_CommApi_TopicCreate
函数功能：创建一个主题
 参数.一：pSt_DDSProtocol
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入主题信息,输出床边的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDDSMessage_CommApi::DDSMessage_CommApi_TopicCreate(XENGINE_PROTOCOL_XDDS* pSt_DDSProtocol)
{
    XDDS_IsErrorOccur = FALSE;

    if (NULL == pSt_DDSProtocol)
    {
        XDDS_IsErrorOccur = TRUE;
        XDDS_dwErrorCode = ERROR_XENGINE_MQCORE_XDDS_COMMAPI_TOPICCREATE_PARAMENT;
        return FALSE;
    }
    pSt_DDSProtocol->bCreater = TRUE;
    //查找主题
    st_Locker.lock();
    unordered_map<tstring, list<XENGINE_PROTOCOL_XDDS> >::iterator stl_MapIterator = stl_MapDDSClient.find(pSt_DDSProtocol->tszTopic);
    if (stl_MapIterator == stl_MapDDSClient.end())
    {
		//不存在,创建,所有信息由创建者提供
        list<XENGINE_PROTOCOL_XDDS> stl_ListDDSProtocol;
        stl_ListDDSProtocol.push_back(*pSt_DDSProtocol);
        stl_MapDDSClient.insert(make_pair(pSt_DDSProtocol->tszTopic, stl_ListDDSProtocol));
    }
    else
    {
		//存在,加入
        stl_MapIterator->second.push_back(*pSt_DDSProtocol);
    }
    st_Locker.unlock();
    return TRUE;
}
/************************************************************************
函数名称：DDSMessage_CommApi_TopicDelete
函数功能：删除一个主题
 参数.一：pSt_DDSProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要删除的主题信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
BOOL CDDSMessage_CommApi::DDSMessage_CommApi_TopicDelete(XENGINE_PROTOCOL_XDDS* pSt_DDSProtocol)
{
    XDDS_IsErrorOccur = FALSE;

    st_Locker.lock();
    unordered_map<tstring, list<XENGINE_PROTOCOL_XDDS> >::iterator stl_MapIterator = stl_MapDDSClient.find(pSt_DDSProtocol->tszTopic);
    if (stl_MapIterator != stl_MapDDSClient.end())
    {
        for (auto stl_ListIterator = stl_MapIterator->second.begin(); stl_ListIterator != stl_MapIterator->second.end(); stl_ListIterator++)
        {
            //只有条件满足才删除.
            if ((0 == _tcsncmp(pSt_DDSProtocol->tszDDSAddr, stl_ListIterator->tszDDSAddr, _tcslen(pSt_DDSProtocol->tszTopic))) && (pSt_DDSProtocol->nPort == stl_ListIterator->nPort) && (pSt_DDSProtocol->bTcp == stl_ListIterator->bTcp))
            {
                stl_MapIterator->second.erase(stl_ListIterator);
            }
        }
        //如果没有了,那就删除
        if (stl_MapIterator->second.empty())
        {
            stl_MapDDSClient.erase(stl_MapIterator);
        }
    }
	st_Locker.unlock();

    return TRUE;
}
/********************************************************************
函数名称：DDSMessage_CommApi_TopicGet
函数功能：获得主题关联信息
 参数.一：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要获取的主题名称
 参数.二：pppSt_DDSProtocol
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出主题关联列表,这个值需要释放内存
 参数.三：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出获取的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CDDSMessage_CommApi::DDSMessage_CommApi_TopicGet(LPCTSTR lpszTopicName, XENGINE_PROTOCOL_XDDS*** pppSt_DDSProtocol, int* pInt_ListCount)
{
	XDDS_IsErrorOccur = FALSE;

	if (NULL == pppSt_DDSProtocol)
	{
		XDDS_IsErrorOccur = TRUE;
		XDDS_dwErrorCode = ERROR_XENGINE_MQCORE_XDDS_COMMAPI_TOPICGET_PARAMENT;
		return FALSE;
	}
    st_Locker.lock_shared();
	unordered_map<tstring, list<XENGINE_PROTOCOL_XDDS> >::iterator stl_MapIterator = stl_MapDDSClient.find(lpszTopicName);
	if (stl_MapIterator == stl_MapDDSClient.end())
	{
		XDDS_IsErrorOccur = TRUE;
		XDDS_dwErrorCode = ERROR_XENGINE_MQCORE_XDDS_COMMAPI_TOPICGET_NOTTOPIC;
		st_Locker.unlock_shared();
		return FALSE;
	}
    *pInt_ListCount = stl_MapIterator->second.size();
    BaseLib_OperatorMemory_Malloc((XPPPMEM)pppSt_DDSProtocol, stl_MapIterator->second.size(), sizeof(XENGINE_PROTOCOL_XDDS));

    auto stl_ListIterator = stl_MapIterator->second.begin();
    for (int i = 0; stl_ListIterator != stl_MapIterator->second.end(); stl_ListIterator++, i++)
    {
        *(*pppSt_DDSProtocol)[i] = *stl_ListIterator;
    }
    st_Locker.unlock_shared();
    return TRUE;
}